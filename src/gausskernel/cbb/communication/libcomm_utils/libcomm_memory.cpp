/*
 * Copyright (c) 2020 Huawei Technologies Co.,Ltd.
 *
 * openGauss is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 * -------------------------------------------------------------------------
 *
 * libcomm_memory.cpp
 *
 * IDENTIFICATION
 *    src/gausskernel/cbb/communication/libcomm_utils/libcomm_memory.cpp
 *
 * -------------------------------------------------------------------------
 */
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <libcgroup.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <net/if.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/time.h>
#include <unistd.h>

#include "../libcomm_core/mc_tcp.h"
#include "../libcomm_core/mc_poller.h"
#include "../libcomm_utils/libcomm_thread.h"
#include "../libcomm_common.h"
#include "libcomm_lqueue.h"
#include "libcomm_queue.h"
#include "libcomm_lock_free_queue.h"
#include "distributelayer/streamCore.h"
#include "distributelayer/streamProducer.h"
#include "pgxc/poolmgr.h"
#include "libpq/auth.h"
#include "libpq/pqsignal.h"
#include "storage/ipc.h"
#include "utils/ps_status.h"
#include "utils/dynahash.h"

#include "vecexecutor/vectorbatch.h"
#include "vecexecutor/vecnodes.h"
#include "executor/exec/execStream.h"
#include "miscadmin.h"
#include "gssignal/gs_signal.h"
#include "pgxc/pgxc.h"

#ifdef ENABLE_UT
#define static
#endif


#define STREAM_SCAN_FINISH 'F'
#define STREAM_SCAN_WAIT 'W'
#define STREAM_SCAN_DATA 'D'

// /* 返回 >=0 节点号，返回 -1 表示未知/失败 */
// static int query_memory_numa_node(void *addr, size_t len)
// {
//     if (addr == NULL || len == 0) return -1;

//     long page_size = sysconf(_SC_PAGESIZE);
//     if (page_size <= 0) return -1;

//     unsigned long start = (unsigned long)addr;
//     unsigned long end = start + len - 1;
//     unsigned long first_page = start & ~(page_size - 1);
//     unsigned long last_page  = end   & ~(page_size - 1);

//     unsigned long npages = (last_page - first_page) / page_size + 1;
//     if (npages == 0) return -1;

//     /* 限制采样页数，避免开销过大 */
//     unsigned long max_sample = 16;
//     unsigned long step = 1;
//     if (npages > max_sample) step = npages / max_sample;

//     unsigned long alloc_pages = (npages + step - 1) / step;
//     if (alloc_pages == 0) return -1;

//     void **pages = (void**)malloc(sizeof(void*) * alloc_pages);
//     int *status   = (int*) malloc(sizeof(int) * alloc_pages);
//     if (!pages || !status) { free(pages); free(status); return -1; }
//     memset(status, 0, sizeof(int) * alloc_pages);

//     unsigned long idx = 0;
//     for (unsigned long p = 0; p < npages && idx < alloc_pages; p += step) {
//         unsigned long off = first_page + p * page_size;
//         pages[idx++] = (void*)off;
//     }

//     /* syscall: pid=0 (self). nodes=NULL -> will fill status[] with node id or negative err. */
//     long rc = syscall(SYS_move_pages, 0, (long)idx, pages, NULL, status, 0);
//     int result = -1;
//     if (rc == 0) {
//         /* 统计众数 */
//         int counts[128];
//         memset(counts, 0, sizeof(counts));
//         int maxnode = -1, maxcnt = 0;
//         for (unsigned long i = 0; i < idx; ++i) {
//             if (status[i] >= 0 && status[i] < (int)(sizeof(counts)/sizeof(counts[0]))) {
//                 counts[status[i]]++;
//                 if (counts[status[i]] > maxcnt) {
//                     maxcnt = counts[status[i]];
//                     maxnode = status[i];
//                 }
//             }
//         }
//         if (maxnode >= 0) result = maxnode;
//         else result = -1;
//     } else {
//         /* move_pages 失败（权限/内核限制等），返回 -1 */
//         result = -1;
//     }

//     free(pages);
//     free(status);
//     return result;
// }
static int query_memory_numa_node(void *addr, size_t len)
{
    if (addr == NULL || len == 0)
        return -1;

    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0)
        return -1;

    unsigned long start = (unsigned long)addr;
    unsigned long end = start + len - 1;

    // 页对齐
    unsigned long first_page = start & ~(page_size - 1);
    unsigned long last_page  = end   & ~(page_size - 1);

    // 总页数
    unsigned long npages = (last_page - first_page) / page_size + 1;
    if (npages == 0)
        return -1;

    // 分配 pages 和 status 数组
    void **pages = (void**)malloc(sizeof(void*) * npages);
    int  *status = (int*) malloc(sizeof(int) * npages);

    if (!pages || !status) {
        free(pages);
        free(status);
        return -1;
    }

    memset(status, 0, sizeof(int) * npages);

    // 逐页填充
    for (unsigned long i = 0; i < npages; ++i) {
        pages[i] = (void*)(first_page + i * page_size);
    }

    // move_pages 查询
    long rc = syscall(SYS_move_pages, 0, (long)npages,
                      pages, NULL, status, 0);

    int result = -1;

    if (rc == 0) {
        // 统计众数（哪个 NUMA node 出现次数最多）
        int counts[128];
        memset(counts, 0, sizeof(counts));

        int maxnode = -1, maxcnt = 0;

        for (unsigned long i = 0; i < npages; ++i) {
            if (status[i] >= 0 && status[i] < 128) {
                counts[status[i]]++;

                if (counts[status[i]] > maxcnt) {
                    maxcnt = counts[status[i]];
                    maxnode = status[i];
                }
            }
        }

        if (maxnode >= 0)
            result = maxnode;
    }

    free(pages);
    free(status);
    return result;
}


extern bool executorEarlyStop();

/* release memory of communication layer, just for LLT */
int gs_release_comm_memory()
{
    AutoContextSwitch commContext(g_instance.comm_cxt.comm_global_mem_cxt);
    gs_r_release_comm_memory();
    return 0;
}

/*
 * @Description: Add stream key to g_r_htab_nodeid_skey_to_memory_poll
 *
 * @param[IN] key_s: stream key
 */
void gs_memory_init_entry(StreamSharedContext* sharedContext, int consumerNum, int producerNum)
{
    struct hash_entry* entry = NULL;
    struct hash_entry** poll_entrys = NULL;
    struct hash_entry*** quota_entrys = NULL;

    poll_entrys = (struct hash_entry**)palloc(sizeof(struct hash_entry*) * consumerNum);
    quota_entrys = (struct hash_entry***)palloc(sizeof(struct hash_entry**) * consumerNum);
    sharedContext->processed_batches = (uint64**)palloc(sizeof(uint64*) * consumerNum);
    sharedContext->processed_rows = (uint64**)palloc(sizeof(uint64*) * consumerNum);
    for (int i = 0; i < consumerNum; i++) {
        sharedContext->processed_batches[i] = (uint64*)palloc(sizeof(uint64) * producerNum);
        sharedContext->processed_rows[i] = (uint64*)palloc(sizeof(uint64) * producerNum);
        entry = (struct hash_entry*)palloc(sizeof(struct hash_entry));
        (void)entry->_init();
        poll_entrys[i] = entry;
        quota_entrys[i] = (struct hash_entry**)palloc(sizeof(struct hash_entry*) * producerNum);
        for (int j = 0; j < producerNum; j++) {
            entry = (struct hash_entry*)palloc(sizeof(struct hash_entry));
            (void)entry->_init();
            quota_entrys[i][j] = entry;
        }
    }

    sharedContext->poll_entrys = poll_entrys;
    sharedContext->quota_entrys = quota_entrys;

}

/*
 * @Description: Send Error/Notice through memory
 *
 * @param[IN] buf: Error/Notice string info
 * @param[IN] sharedContext: context for shared memory stream
 * @param[IN] nthChannel: destination consumer
 */
void gs_message_by_memory(StringInfo buf, StreamSharedContext* sharedContext, int nthChannel)
{
    StringInfo buf_dst = NULL;
    struct hash_entry* entry = NULL;

    /* Copy Error/Notice messages to shared context. */
    buf_dst = sharedContext->messages[nthChannel][u_sess->stream_cxt.smp_id];

    /*
     * If producer is waked up and shared buffer has been consumed while waiting,
     * it can continue to append data to its messages of sharedContext.
     */
    entry = sharedContext->quota_entrys[nthChannel][u_sess->stream_cxt.smp_id];
    while (buf_dst->len > 0) {
        (void)entry->_timewait(SINGLE_WAITQUOTA);
    }
    appendBinaryStringInfo(buf_dst, buf->data, buf->len);
    buf_dst->cursor = buf->cursor;

    /* Send signal to dest consumer. */
    entry = sharedContext->poll_entrys[nthChannel];
    entry->_signal();

    pfree(buf->data);
    buf->data = NULL;
}

void gs_memory_disconnect(StreamSharedContext* sharedContext, int nthChannel)
{
    struct hash_entry* entry = NULL;
    sharedContext->dataStatus[nthChannel][u_sess->stream_cxt.smp_id] = CONN_ERR;
    entry = sharedContext->poll_entrys[nthChannel];
    entry->_signal();
}

#ifdef __aarch64__
/*
 * @Description: Judge whether the databuff is empty
 *
 * @param[IN] sharedContext: context for shared memory stream
 * @param[IN] nthChannel: destination consumer
 */
bool gs_is_databuff_empty(StreamSharedContext* sharedContext, int nthChannel)
{
    if (sharedContext->vectorized) {
        VectorBatch* batch = sharedContext->sharedBatches[nthChannel][u_sess->stream_cxt.smp_id];
        if (batch->m_rows == 0) {
            return true;
        }
    } else {
        TupleVector* tupleVec = sharedContext->sharedTuples[nthChannel][u_sess->stream_cxt.smp_id];
        if (tupleVec->tuplePointer == 0) {
            return true;
        }
    }
    return false;
}
#endif

/*
 * @Description: Send data to local consumer through shared memory
 *
 * @param[IN] tuple: tuple to be sent
 * @param[IN] batchsrc: batch to be send
 * @param[IN] sharedContext: context for shared memory stream
 * @param[IN] nthChannel: destination consumer
 * @param[IN] nthRow: the Nth row to be sent in batch
 */
void gs_memory_send(
    TupleTableSlot* tuple, VectorBatch* batchsrc, StreamSharedContext* sharedContext, int nthChannel, int nthRow)
{
    VectorBatch* batch = NULL;
    TupleVector* tupleVec = NULL;
    bool ready_to_send = false;
    DataStatus dataStatus;
    struct hash_entry* entry = NULL;

    WaitState oldStatus = pgstat_report_waitstatus_comm(STATE_WAIT_FLUSH_DATA,
        u_sess->pgxc_cxt.PGXCNodeId,
        -1,
        u_sess->stream_cxt.producer_obj->getParentPlanNodeId(),
        global_node_definition ? global_node_definition->num_nodes : -1);
    // struct timeval stream_start, stream_end;
    // struct timeval copy_start, copy_end;
    // gettimeofday(&stream_start, NULL);
    StreamTimeSendStart(t_thrd.pgxc_cxt.GlobalNetInstr);
    entry = sharedContext->quota_entrys[nthChannel][u_sess->stream_cxt.smp_id];
    for (;;) {
        /* Check for interrupt at the beginning of the loop. */
        CHECK_FOR_INTERRUPTS();

        /* Check if we should early stop. */
        /* Quit if the connection close, especially in a early close case. */
        if (executorEarlyStop() || sharedContext->is_connect_end[nthChannel][u_sess->stream_cxt.smp_id]) {
            (void)pgstat_report_waitstatus(oldStatus);
            return;
        }

        dataStatus = sharedContext->dataStatus[nthChannel][u_sess->stream_cxt.smp_id];
        /* Break the loop if we find quota. */
        if ((dataStatus == DATA_EMPTY
#ifdef __aarch64__
             && gs_is_databuff_empty(sharedContext, nthChannel)
#endif
             ) ||
            dataStatus == DATA_PREPARE) {
            break;
        }

        StreamTimeWaitQuotaStart(t_thrd.pgxc_cxt.GlobalNetInstr);
        (void)entry->_timewait(SINGLE_WAITQUOTA);
        StreamTimeWaitQuotaEnd(t_thrd.pgxc_cxt.GlobalNetInstr);
    }
    // gettimeofday(&stream_end, NULL);
    // gettimeofday(&copy_start, NULL);
    StreamTimeCopyStart(t_thrd.pgxc_cxt.GlobalNetInstr);
    // u_sess->stream_cxt.trace_cache_obj->start(u_sess->stream_cxt.producer_obj->m_streamNode->scan.plan.plan_node_id);
    // struct timeval copy_start, copy_end;
    // struct timeval numa_start, numa_end;
    // gettimeofday(&copy_start, NULL);
    /* Copy data to shared context. */
    if (sharedContext->vectorized) {
        Assert(sharedContext->sharedBatches != NULL);
        batch = sharedContext->sharedBatches[nthChannel][u_sess->stream_cxt.smp_id];

        /*===========================================================
        ⚠️ 先暂停计时：进入 NUMA 采样前记录时间
        ===========================================================*/
        // gettimeofday(&numa_start, NULL);

        // /* ================= NUMA 采样开始 ================= */
        // // 当前线程 CPU
        // int src_cpu = sched_getcpu();
        // // 映射 NUMA
        // int src_numa = cpu_to_numa_node(src_cpu);
        
        // pid_t provtid = syscall(SYS_gettid);

        // batch->producer_cpu  = src_cpu;
        // batch->producer_numa = src_numa;
        // batch->producer_tid = provtid;

        // /* 按需为列级 numa 分配数组（如果还没分配） */
        // if (batch->producer_col_numa == NULL && batchsrc->m_cols > 0) {
        //     batch->producer_col_numa = (int*)malloc(sizeof(int) * batchsrc->m_cols);
        //     if (batch->producer_col_numa) {
        //         for (int c = 0; c < batchsrc->m_cols; ++c)
        //             batch->producer_col_numa[c] = -1;
        //     }
        // }

        // /* 遍历每列，采样该列数据的主 NUMA（基于 ScalarVector::m_vals） */
        // for (int c = 0; c < batchsrc->m_cols; ++c) {
        //     int col_node = -1;
        //     ScalarVector *sv = NULL;

        //     /* 访问方式依据 m_arr 类型：你的定义是 ScalarVector* m_arr;
        //        如果 m_arr 是列数组，取 &m_arr[c]；如果是指针数组请改为 m_arr[c] */
        //     sv = &batchsrc->m_arr[c];

        //     if (sv && sv->m_vals) {
        //         size_t datalen = (size_t)batchsrc->m_rows * sizeof(ScalarValue);
        //         if (datalen > 0)
        //             col_node = query_memory_numa_node((void*)sv->m_vals, datalen);
        //     }

        //     if (batch->producer_col_numa)
        //         batch->producer_col_numa[c] = col_node;

        //     /* 如果总体 numa 未设置，优先用第一列结果作为代表 */
        //     if (batch->producer_numa < 0 && col_node >= 0)
        //         batch->producer_numa = col_node;
        // }
        // /* ====== 记录结束 ====== */
        // /* ================= NUMA 采样结束 ================= */

        // /*===========================================================
        // ⚠️ 恢复计时：减掉 NUMA 采样耗时
        // ===========================================================*/
        // gettimeofday(&numa_end, NULL);

        // /* 重新设置 copy_start，使 NUMA 时间不计入 copy 的测量 */
        // copy_start.tv_sec  += (numa_end.tv_sec  - numa_start.tv_sec);
        // copy_start.tv_usec += (numa_end.tv_usec - numa_start.tv_usec);
        /*===========================================================*/

        /* data copy */
        if (-1 == nthRow) {
            /* Do deep copy of all rows, for local roundrobin & local broadcast. */
            Assert(batch->m_rows == 0);
            batch->Copy<true, false>(batchsrc);
            ready_to_send = true;
            // sharedContext->processed_batches[nthChannel][u_sess->stream_cxt.smp_id]++;
            // sharedContext->processed_rows[nthChannel][u_sess->stream_cxt.smp_id] += batchsrc->m_rows;
        } else {
            batch->CopyNth(batchsrc, nthRow);
            // sharedContext->processed_batches[nthChannel][u_sess->stream_cxt.smp_id]++;
            // sharedContext->processed_rows[nthChannel][u_sess->stream_cxt.smp_id] += batchsrc->m_rows;
            if (BatchMaxSize == batch->m_rows) {
                ready_to_send = true;
            }
        }
    } else {
        Assert(sharedContext->sharedTuples != NULL);
        tupleVec = sharedContext->sharedTuples[nthChannel][u_sess->stream_cxt.smp_id];
        int n = tupleVec->tuplePointer;
        ExecCopySlot(tupleVec->tupleVector[n], tuple);
        tupleVec->tuplePointer++;
        if (TupleVectorMaxSize == tupleVec->tuplePointer) {
            ready_to_send = true;
        }
    }
    // gettimeofday(&copy_end, NULL);
    // double elapsed = (copy_end.tv_sec - copy_start.tv_sec) * 1e6 +
    //                 (copy_end.tv_usec - copy_start.tv_usec);
    // size_t bytes = batchsrc->m_rows * batchsrc->m_cols * sizeof(ScalarValue);

    // if (u_sess->stream_cxt.producer_obj &&
    // u_sess->stream_cxt.producer_obj->m_sendMonitor) {
    // u_sess->stream_cxt.producer_obj->m_sendMonitor->AddSendStat(bytes, elapsed);
    // }
    // u_sess->stream_cxt.trace_cache_obj->stop();
    StreamTimeCopyEnd(t_thrd.pgxc_cxt.GlobalNetInstr);
    //每个batch都要先加锁，确保对信号量状态的修改和线程等待计数的操作是原子安全的。if (waiting_count > 0) { LIBCOMM_PTHREAD_COND_SIGNAL(&cond); }：如果有线程正在等待该信号量（waiting_count 记录等待线程数），则通过条件变量 cond 唤醒其中一个等待线程，让它可以继续执行（获取信号量）。最后解锁，允许其他线程操作信号量。
    //统计唤醒次数和唤醒开销
    /* send the signal if copy finished */
    if (ready_to_send) {
#ifdef __aarch64__
        pg_memory_barrier();
#endif
        /* set flag */
        sharedContext->dataStatus[nthChannel][u_sess->stream_cxt.smp_id] = DATA_READY;
        /* send signal */
        entry = sharedContext->poll_entrys[nthChannel];
        entry->_signal();
    } else {
        sharedContext->dataStatus[nthChannel][u_sess->stream_cxt.smp_id] = DATA_PREPARE;
    }
    StreamTimeSendEnd(t_thrd.pgxc_cxt.GlobalNetInstr);

    (void)pgstat_report_waitstatus(oldStatus);
}

/*
 * @Description: catch a tuple from stream's buffer.
 *
 * @param[IN] node: stream state
 * @return bool: true -- found data
 */
FORCE_INLINE
bool gs_return_tuple(StreamState* node)
{
    TupleVector* tupleVec = node->tempTupleVec;

    if (tupleVec->tuplePointer == tupleVec->tupleCount) {
        return false;
    }

    int n = tupleVec->tuplePointer;
    node->ss.ps.ps_ResultTupleSlot = tupleVec->tupleVector[n];
    tupleVec->tuplePointer++;

    return true;
}

/*
 * @Description: Consume the data in shared memory from local producers.
 *
 * @param[IN] node: stream state
 * @param[IN] loc: data location
 * @return bool: true -- found data
 */
bool gs_consume_memory_data(StreamState* node, int loc)
{
    StreamSharedContext* sharedContext = node->sharedContext;

    NetWorkTimeCopyStart(t_thrd.pgxc_cxt.GlobalNetInstr);

    // struct timeval copy_start, copy_end;
    // struct timeval numa_start, numa_end;
    // gettimeofday(&copy_start, NULL);
    // size_t bytes = 0;   /* <- 先默认 0，只有 vectorized 时设置 */
    /* Take data from the shared context. */
    if (sharedContext->vectorized) {
        VectorBatch* batchsrc = sharedContext->sharedBatches[u_sess->stream_cxt.smp_id][loc];
        VectorBatch* batchdst = ((VecStreamState*)node)->m_CurrentBatch;

        if (batchsrc->m_rows == 0) {
            return false;
        }

        batchdst->Copy<true, false>(batchsrc);

        // /*===========================================================
        // ⚠️ 先暂停计时：进入 NUMA 采样前记录时间
        // ===========================================================*/
        // gettimeofday(&numa_start, NULL);

        // /* ================= NUMA 采样开始 ================= */

        // 获取目标 CPU/NUMA
        // int dst_cpu  = sched_getcpu();
        // int dst_numa = cpu_to_numa_node(dst_cpu);

        // // 获取生产端 CPU/NUMA
        // int src_cpu  = batchsrc->producer_cpu;
        // int src_numa = batchsrc->producer_numa;  
        // int src_pid   = batchsrc->producer_tid;

        // /* 准备打印前几个列的列级 NUMA（避免日志太长） */
        // int show_cols = batchsrc->m_cols;  
        // StringInfoData tmp;
        // initStringInfo(&tmp);
        // for (int cc = 0; cc < show_cols; ++cc) {
        //     int colnode = -1;
        //     if (batchsrc->producer_col_numa) colnode = batchsrc->producer_col_numa[cc];
        //     appendStringInfo(&tmp, "col%d_node=%d ", cc, colnode);
        // }
        // pid_t recvtid = syscall(SYS_gettid);
        // elog(LOG,
        //      "[BatchPath] PROTID=%d RECVTID=%d smp=%d loc=%d src={cpu=%d numa=%d} dst={cpu=%d numa=%d} rows=%d cols=%d %s",
        //      src_pid,
        //      recvtid,
        //      u_sess->stream_cxt.smp_id, loc,
        //      src_cpu, src_numa,
        //      dst_cpu, dst_numa,
        //      batchsrc->m_rows, batchsrc->m_cols,
        //      tmp.data);
    //     elog(LOG,
    //  "[BatchPath] smp=%d loc=%d src={cpu=%d numa=%d} dst={cpu=%d numa=%d} rows=%d cols=%d %s sizeof(ScalarValue)=%zu",
    //  u_sess->stream_cxt.smp_id, loc,
    //  src_cpu, src_numa,
    //  dst_cpu, dst_numa,
    //  batchsrc->m_rows, batchsrc->m_cols,
    //  tmp.data,
    //  sizeof(ScalarValue));

        // if (tmp.data) pfree(tmp.data);

        // /* ================= NUMA 采样结束 ================= */

        // /*===========================================================
        // ⚠️ 恢复计时：减掉 NUMA 采样耗时
        // ===========================================================*/
        // gettimeofday(&numa_end, NULL);

        // /* 重新设置 copy_start，使 NUMA 时间不计入 copy 的测量 */
        // copy_start.tv_sec  += (numa_end.tv_sec  - numa_start.tv_sec);
        // copy_start.tv_usec += (numa_end.tv_usec - numa_start.tv_usec);
        /*===========================================================*/

        /* 只有 VectorBatch 模式才计算 bytes */
        // bytes = batchdst->m_rows * batchdst->m_cols * sizeof(ScalarValue);

        batchsrc->Reset();
    } else {
        TupleVector* tuplesrc = sharedContext->sharedTuples[u_sess->stream_cxt.smp_id][loc];
        TupleVector* tupledst = node->tempTupleVec;

        if (tuplesrc->tuplePointer == 0) {
            return false;
        }

        for (int i = 0; i < tuplesrc->tuplePointer; i++) {
            (void)ExecCopySlot(tupledst->tupleVector[i], tuplesrc->tupleVector[i]);
        }

        tupledst->tupleCount = tuplesrc->tuplePointer;
        tupledst->tuplePointer = 0;
        tuplesrc->tuplePointer = 0;
        (void)gs_return_tuple(node);
    }
    // gettimeofday(&copy_end, NULL);
    // double elapsed = (copy_end.tv_sec - copy_start.tv_sec) * 1e6 +
    //                 (copy_end.tv_usec - copy_start.tv_usec);

    // if (u_sess->stream_cxt.producer_obj &&
    // u_sess->stream_cxt.producer_obj->m_recvMonitor) {
    // u_sess->stream_cxt.producer_obj->m_recvMonitor->AddSendStat(bytes, elapsed);
    // }
    NetWorkTimeCopyEnd(t_thrd.pgxc_cxt.GlobalNetInstr);

    struct hash_entry* entry = NULL;
    entry = sharedContext->quota_entrys[u_sess->stream_cxt.smp_id][loc];

#ifdef __aarch64__
    pg_memory_barrier();
#endif
    /* Reset flag */
    sharedContext->dataStatus[u_sess->stream_cxt.smp_id][loc] = DATA_EMPTY;

    /* send signal */
    entry->_signal();

    node->sharedContext->scanLoc[u_sess->stream_cxt.smp_id] = loc;
    return true;
}

/*
 * @Description: Scan the producer status to find the data.
 *
 * @param[IN] node: stream state
 * @return char: STREAM_SCAN_DATA -- successfully find data from producer.
 *                  STREAM_SCAN_WAIT -- still need to poll to wait for data.
 *                 STREAM_SCAN_FINISH -- stream scan finished.
 */
char gs_find_memory_data(StreamState* node, int* waitnode_count)
{
    DataStatus dataStatus;
    StringInfo buf = NULL;
    int scanLoc = node->sharedContext->scanLoc[u_sess->stream_cxt.smp_id];
    int i = scanLoc;
    bool finished = true;
    bool is_conn_end = false;
    int waitnodeCount = 0;
    struct hash_entry* entry = NULL;
    
    /* Check if there is available data, and scan from last time location. */
    do {
        i++;
        if (i == node->conn_count) {
            i = 0;
        }

        /* Update scan location. */
        node->sharedContext->scanLoc[u_sess->stream_cxt.smp_id] = i;
        dataStatus = node->sharedContext->dataStatus[u_sess->stream_cxt.smp_id][i];
        is_conn_end = node->sharedContext->is_connect_end[u_sess->stream_cxt.smp_id][i];

        if (!is_conn_end) {
            finished = false;
            waitnodeCount++;
        }

        /*
         * Firstly, we handle error or notice messages.
         * If an error occured, we should stop scan now.
         * If an notice occured, we can still receive data.
         */
        buf = node->sharedContext->messages[u_sess->stream_cxt.smp_id][i];

        if (buf->cursor == 'R') {
            node->ss.ps.state->es_processed += node->sharedContext->rows;
            resetStringInfo(buf);
        }

        if (buf->len > 0) {
            if (buf->cursor == 'E') {
                HandleStreamError(node, buf->data, buf->len);
                return STREAM_SCAN_FINISH;
            } else if (buf->cursor == 'N') {
                HandleStreamNotice(node, buf->data, buf->len);
                resetStringInfo(buf);

                /* After one notice message has handled, send signal and wake up the dest producer. */
                entry = node->sharedContext->quota_entrys[u_sess->stream_cxt.smp_id][i];
                entry->_signal();

                return STREAM_SCAN_WAIT;
            }
        }

        switch (dataStatus) {
            case DATA_EMPTY:
                break;

            case DATA_PREPARE:
                /* Take the rest data away when the connection is end. */
                if (is_conn_end) {
                    /* Return data if any. */
                    if (gs_consume_memory_data(node, i)) {
                        return STREAM_SCAN_DATA;
                    }
                }
                break;

            case DATA_READY:
                if (gs_consume_memory_data(node, i)) {
                    return STREAM_SCAN_DATA;
                } else {
                    break;
                }

            case CONN_ERR:
                ereport(ERROR,
                    (errcode(ERRCODE_STREAM_REMOTE_CLOSE_SOCKET),
                        errmsg("Failed to read response from Local Stream Node,"
                               " Detail: Node %s, Plan Node ID %u, SMP ID %d",
                            g_instance.attr.attr_common.PGXCNodeName,
                            node->sharedContext->key_s.planNodeId,
                            i)));
                break;
            // dataStatus is enum,
            default:
                break;
        }
    } while (i != scanLoc);

    *waitnode_count = waitnodeCount;

    if (finished) {
        return STREAM_SCAN_FINISH;
    } else {
        return STREAM_SCAN_WAIT;
    }
}

/*
 * @Description: Receive data from shared memory for local stream.
 *
 * @param[IN] node: stream state
 * @return bool: true -- successed to find data and need more data.
 *                  false -- all connection finished or recerive error.
 */
bool gs_memory_recv(StreamState* node)
{
    char result;
    struct hash_entry* entry = NULL;
    entry = node->sharedContext->poll_entrys[u_sess->stream_cxt.smp_id];
    bool re = true;
    int waitnode_count = 0;

    /* If there is already tuple in buffer, return the data at once. */
    if (!node->sharedContext->vectorized && gs_return_tuple(node)) {
        return true;
    }

    for (;;) {
        /* Check for interrupt at the beginning of the loop. */
        CHECK_FOR_INTERRUPTS();

        /* Check if we can early stop now. */
        if (executorEarlyStop()) {
            re = false;
            break;
        }

        /* Search all producers to find data. */
        result = gs_find_memory_data(node, &waitnode_count);
        if (result == STREAM_SCAN_DATA) {
            re = true;
            break;
        } else if (result == STREAM_SCAN_FINISH) {
            re = false;
            break;
        }

        WaitStatePhase oldPhase = pgstat_report_waitstatus_phase(PHASE_NONE, true);
        WaitState oldStatus = pgstat_report_waitstatus_comm(STATE_WAIT_NODE,
            u_sess->pgxc_cxt.PGXCNodeId,
            waitnode_count,
            node->sharedContext->key_s.planNodeId,
            global_node_definition ? global_node_definition->num_nodes : -1);

        /* Poll to wait data from producers. */
        NetWorkTimePollStart(t_thrd.pgxc_cxt.GlobalNetInstr);
        (void)entry->_timewait(SINGLE_WAITQUOTA);
        NetWorkTimePollEnd(t_thrd.pgxc_cxt.GlobalNetInstr);

        pgstat_reset_waitStatePhase(oldStatus, oldPhase);
    }

    return re;
}

/*
 * @Description: Inform all related consuemrs that there is no more data to send.
 *
 * @param[IN] sharedContext: context for shared memory stream
 * @param[IN] connNum: producer connection number
 */
void gs_memory_send_finish(StreamSharedContext* sharedContext, int connNum)
{
    struct hash_entry* entry = NULL;

    for (int i = 0; i < connNum; i++) {
        /* Set flags. */
        sharedContext->is_connect_end[i][u_sess->stream_cxt.smp_id] = true;

        /* send signal */
        entry = sharedContext->poll_entrys[i];
        entry->_signal();
    }
    
}

/*
 * @Description: Set all connections with this producer to close.
 *
 * @param[IN] sharedContext: context for shared memory stream
 * @param[IN] connNum: producer connection number
 * @param[IN] smpId: producer smp id
 */
void gs_memory_close_conn(StreamSharedContext* sharedContext, int connNum, int consumerId)
{
    struct hash_entry* entry = NULL;

    for (int i = 0; i < connNum; i++) {
        /* Set flags. */
        sharedContext->is_connect_end[consumerId][i] = true;
        // uint64 total_batches = 0;
        // uint64 total_rows = 0;
        // // int active_threads = 0;
        
        // elog(LOG, "Stream Copy Statistics for producer %d:", i);
        
        // for (int smp_id = 0; smp_id < i; smp_id++) {
            // uint64 batches = sharedContext->processed_batches[consumerId][i];
            // uint64 rows = sharedContext->processed_rows[consumerId][i];
            // elog(LOG,"stream parent_node %d",u_sess->stream_cxt.producer_obj->getParentPlanNodeId());
            
            // if (batches > 0) {
            //     elog(LOG, "  SMP%d: %lu batches, %lu rows (avg %.2f rows/batch)", 
            //          i, batches, rows, rows / (double)batches);
            //     // total_batches += batches;
            //     // total_rows += rows;
            // }
        // }
        
        // if (total_batches > 0) {
        //     elog(LOG, "Channel %d Summary: %lu total batches, %lu total rows, %d active threads, "
        //          "average %.2f batches/thread", 
        //          consumerId);
        // // }
        /*
         * Send signal to the producers which may be still waiting quota,
         * in a query like "limit XXX", when consumer don't need data anymore,
         * but the producers haven't send all data yet.
         */
        entry = sharedContext->quota_entrys[consumerId][i];
        entry->_signal();
    }
    // u_sess->stream_cxt.trace_cache_obj->print_average_stats();
}

