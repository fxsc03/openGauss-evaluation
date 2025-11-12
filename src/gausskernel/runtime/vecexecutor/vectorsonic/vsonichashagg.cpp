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
 * vsonichashagg.cpp
 * 		Routines to handle vector sonic hashagg nodes. Sonic Hash Agg nodes 	based
 * on the column-based hash table.
 *
 * IDENTIFICATION
 *       Code/src/gausskernel/runtime/vecexecutor/vectorsonic/vsonichashagg.cpp
 *
 * -------------------------------------------------------------------------
 */
#include <chrono>
#include<vector>
#include <unordered_map>
#include <time.h>
#include <set>
#include "vectorsonic/vsonichashagg.h"
#include "vectorsonic/vsonicarray.h"
#include "catalog/pg_aggregate.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_proc.h"
#include "executor/node/nodeAgg.h"
#include "nodes/execnodes.h"
#include "utils/elog.h"
#include "utils/dynahash.h"
#include "utils/int8.h"

/*
 * @Description	: Check if current aggref's expression is supported or not.
 * @in node		: Aggref's expression node.
 * @return		: return true aggref's expression is supported.
 */
bool isExprSonicEnable(Expr* node)
{
    switch (nodeTag(node)) {
        case T_TargetEntry: {
            TargetEntry* tentry = (TargetEntry*)node;
            if (!isExprSonicEnable(tentry->expr)) {
                return false;
            }
            break;
        }
        case T_Var: {
            Var* var = (Var*)node;

            /* If var is in sysattrlist, do not use sonichash */
            if (var->varattno < 0) {
                return false;
            }

            /* only consider int2, int4, int8, numeric type */
            switch (var->vartype) {
                case INT2OID:
                case INT4OID:
                case INT8OID:
                case NUMERICOID:
                    break;
                default:
                    return false;
            }
            break;
        }
        case T_Const: {
            Const* cst = (Const*)node;

            /* only consider int4, int8, numeric const value */
            switch (cst->consttype) {
                case INT4OID:
                case INT8OID:
                case NUMERICOID:
                    break;
                default:
                    return false;
            }
            break;
        }
        case T_FuncExpr: {
            FuncExpr* func_expr = (FuncExpr*)node;
            if (func_expr->funcretset) {
                return false;
            }

            /* only consider several return type */
            switch (func_expr->funcresulttype) {
                case INT2OID:
                case INT4OID:
                case INT8OID:
                case NUMERICOID:
                    break;
                default:
                    return false;
            }

            switch (func_expr->funcid) {
                case INT2SMALLERFUNCOID:
                case INT2LARGERFUNCOID:
                case INT4SMALLERFUNCOID:
                case INT4LARGERFUNCOID:
                case INT8SMALLERFUNCOID:
                case INT8LARGERFUNCOID:
                    break;
                default:
                    return false;
            }
            break;
        }
        case T_CaseExpr: {
            CaseExpr* case_expr = (CaseExpr*)node;
            switch (case_expr->casetype) {
                case INT2OID:
                case INT4OID:
                case INT8OID:
                case NUMERICOID:
                    break;
                default:
                    return false;
            }

            /* If exists case_expr argument, we should consider it */
            if (case_expr->arg != NULL) {
                if (!isExprSonicEnable(case_expr->arg)) {
                    return false;
                }
            }

            /* Consider both when clause and result clause */
            ListCell* cell = NULL;
            foreach (cell, case_expr->args) {
                CaseWhen* wclause = (CaseWhen*)lfirst(cell);
                if (!isExprSonicEnable(wclause->expr) || !isExprSonicEnable(wclause->result)) {
                    return false;
                }
            }

            /* Consider default caluse */
            Expr* case_default = case_expr->defresult;
            if (case_default != NULL) {
                if (!isExprSonicEnable(case_default)) {
                    return false;
                }
            }
            break;
        } 
        case T_OpExpr: {
            OpExpr* op_expr = (OpExpr*)node;
            if (list_length(op_expr->args) == 1)
                return false;

            switch (op_expr->opno) {
                case INT4PLOID:
                case INT4MIOID:
                case INT4MULOID:
                case INT4DIVOID:
                case INT8PLOID:
                case INT8MIOID:
                case INT8MULOID:
                case INT8DIVOID:
                case NUMERICADDOID:
                case NUMERICSUBOID:
                case NUMERICMULOID:
                case NUMERICDIVOID:
                    break;
                default:
                    return false;
            }

            /* make sure all the args can use sonic hashagg */
            Expr* lexpr = (Expr*)linitial(op_expr->args);
            Expr* rexpr = (Expr*)lsecond(op_expr->args);

            if (!isExprSonicEnable(lexpr) || !isExprSonicEnable(rexpr)) {
                return false;
            }
            break;
        }
        default:
            return false;
    }

    return true;
}

/*
 * @Description	: Check if current aggfunction is supported or not.
 * @in aggfnoid	: Oid of the agg function.
 * @return		: Return true if agg function is allowed.
 */
bool isAggrefSonicEnable(Oid aggfnoid)
{
    switch (aggfnoid) {
        case INT2AVGFUNCOID:
        case INT2SUMFUNCOID:
        case INT2SMALLERFUNCOID:
        case INT2LARGERFUNCOID:
        case INT4AVGFUNCOID:
        case INT4SUMFUNCOID:
        case INT4SMALLERFUNCOID:
        case INT4LARGERFUNCOID:
        case INT8AVGFUNCOID:
        case INT8SUMFUNCOID:
        case INT8SMALLERFUNCOID:
        case INT8LARGERFUNCOID:
        case NUMERICAVGFUNCOID:
        case NUMERICSUMFUNCOID:
        case NUMERICSMALLERFUNCOID:
        case NUMERICLARGERFUNCOID:
        case COUNTOID:
        case ANYCOUNTOID:
        case ADDTDIGESTMERGEOID:
        case ADDTDIGESTMERGEPOID:
            return true;
            break;
        default:
            return false;
    }
}

static bool check_sonic_hash_agg_walker(Node* node)
{
    switch (nodeTag(node)) {
        case T_SubPlan: {
            SubPlan* sub_plan = (SubPlan*)node;
            if (sub_plan->testexpr != NULL && IsA(sub_plan->testexpr, OpExpr)) {
                OpExpr* op_expr = (OpExpr*)sub_plan->testexpr;
                List* op_list = op_expr->args;
                ListCell* lop = NULL;
                foreach (lop, op_list) {
                    Expr* op_arg = (Expr*)lfirst(lop);
                    if (IsA(op_arg, Aggref)) {
                        Aggref* op_aggref = (Aggref*)op_arg;
                        if (!isAggrefSonicEnable(op_aggref->aggfnoid)) {
                            return true;
                        }
                    }
                }
            } else {
                return true;
            }
            break;
        }
        case T_OpExpr: {
            OpExpr* op_expr = (OpExpr*)node;
            List* op_args = op_expr->args;
            ListCell* lop = NULL;
            foreach (lop, op_args) {
                Expr* op_arg = (Expr*)lfirst(lop);
                if (IsA(op_arg, Aggref)) {
                    Aggref* op_aggref = (Aggref*)op_arg;
                    if (!isAggrefSonicEnable(op_aggref->aggfnoid)) {
                        return true;
                    }
                }
            }
            break;
        }
        case T_Aggref: {
            Aggref* agg_ref = (Aggref*)node;

            if (!isAggrefSonicEnable(agg_ref->aggfnoid)) {
                return true;
            }

            /* count(*) has no args */
            if (agg_ref->aggfnoid == COUNTOID || agg_ref->aggfnoid == ANYCOUNTOID) {
                break;
            }

            Expr* ref_expr = (Expr*)linitial(agg_ref->args);
            /* We only support simple expression cases */
            if (!isExprSonicEnable(ref_expr)) {
                return true;
            }
            break;
        }
        default: {
            if (!isExprSonicEnable((Expr*) node)) {
                return true;
            }
            return expression_tree_walker(node, (bool (*)())check_sonic_hash_agg_walker, (void*)NULL);
        }
    }

    return expression_tree_walker(node, (bool (*)())check_sonic_hash_agg_walker, (void*)NULL);
}

/*
 * @Description	: Decide use Sonic Hash Agg routine or not.
 * @in agg		: Vector Aggregation Node information.
 * @return		: Return true if sonic hashagg routine can be used.
 */
bool isSonicHashAggEnable(VecAgg* node)
{
    /* Only support hashagg. */
    if (node->aggstrategy != AGG_HASHED || !u_sess->attr.attr_sql.enable_sonic_hashagg) {
        return false;
    }

    /*
     * Aggregate function only support sum(), avg() function for int4, int8 and numeric tyep.
     * Loop over all the targetlist and quallist to check aggref case.
     */
    List* plant_list = node->plan.targetlist;
    ListCell* lc = NULL;
    foreach (lc, plant_list) {
        TargetEntry* tentry = (TargetEntry*)lfirst(lc);
        switch (nodeTag(tentry->expr)) {
            case T_Aggref: {
                Aggref* agg_ref = (Aggref*)tentry->expr;

                if (!isAggrefSonicEnable(agg_ref->aggfnoid)) {
                    return false;
                }

                /* count(*) has no args */
                if (agg_ref->aggfnoid == COUNTOID || agg_ref->aggfnoid == ANYCOUNTOID) {
                    continue;
                }

                Expr* ref_expr = (Expr*)linitial(agg_ref->args);
                /* We only support simple expression cases */
                if (!isExprSonicEnable(ref_expr)) {
                    return false;
                }
                break;
            }
            case T_OpExpr: {
                /* support simple expression in targetlist */
                if (!isExprSonicEnable(tentry->expr)) {
                    return false;
                }
                break;
            }
            case T_FuncExpr: {
                FuncExpr* func_expr = (FuncExpr*)tentry->expr;

                List* fargs = func_expr->args;
                ListCell* list_cell = NULL;
                foreach (list_cell, fargs) {
                    Expr* farg = (Expr*)lfirst(list_cell);
                    if (!IsA(farg, Var) && !IsA(farg, Const)) {
                        if (!isExprSonicEnable(farg)) {
                            return false;
                        }
                    }
                }
                break;
            }
            case T_Var:
            case T_Const:
                /* no constraints */
                break;
            default:
                return false;
                break;
        }
    }

    /* loop over all the qual_list to check condition */
    List* qual_list = node->plan.qual;
    foreach (lc, qual_list) {
        Expr* qual_expr = (Expr*)lfirst(lc);
        if (check_sonic_hash_agg_walker((Node*) qual_expr)) {
            return false;
        }
    }

    return true;
}

/*
 * @Description	: Constructed function for init agg information, sonic data array, result batch
 *				 and sonic hash table.
 * @in node		: Vector aggreate state.
 * @in arrSize		: The array size of sonic atom data arry.
 * @out			: SonicHashAgg infomation.
 */
SonicHashAgg::SonicHashAgg(VecAggState* runtime, int arrSize) : SonicHash(arrSize), m_runtime(runtime)
{
    m_rows = 0;
    m_keySimple = true;
    m_strategy = HASH_IN_MEMORY;
    m_runState = AGG_PREPARE;
    m_sonicHashSource = NULL;
    m_partFileSource = NULL;
    m_overflowFileSource = NULL;
    m_hashbuild_time = 0.0;
    m_calcagg_time = 0.0;
    m_tupleCount = m_colWidth = 0;
    m_enableExpansion = true;
    m_currPartIdx = -1;
    m_arrayElementSize = 0;
    m_arrayExpandSize = 0;
    m_segNum = 0;
    m_segBucket = NULL;

    VecAgg* node = (VecAgg*)(m_runtime->ss.ps.plan);
    m_econtext = m_runtime->ss.ps.ps_ExprContext;

    /* init aggregation information */
    initAggInfo();

    /* init batch information used during hashagg routine */
    initBatch();

    /* initialize memcontrol */
    initMemoryControl();

    m_memControl.hashContext = AllocSetContextCreate(CurrentMemoryContext,
        "SonicHashAggContext",
        ALLOCSET_DEFAULT_MINSIZE,
        ALLOCSET_DEFAULT_INITSIZE,
        ALLOCSET_DEFAULT_MAXSIZE,
        EnableBorrowWorkMemory() ? RACK_CONTEXT : STANDARD_CONTEXT,
        m_memControl.totalMem);

    AddControlMemoryContext(runtime->ss.ps.instrument, m_memControl.hashContext);

    {
        AutoContextSwitch memSwitch(m_memControl.hashContext);

        /* initialize sonic datum array to store table(batch) */
        initDataArray();

        /* compute hash table size that needed */
        m_hashSize = Min(2 * node->numGroups, (long)(m_memControl.totalMem / m_arrayElementSize));
        m_hashSize = calcHashTableSize<false, true>(m_hashSize);

        /* initialize sonic hash table */
        initHashTable();

        /* report initial memory needed by sonic hashagg */
        int64 initNeedSize = 0;
        int64 initFreeSize = 0;
        calcHashContextSize(m_memControl.hashContext, &initNeedSize, &initFreeSize);

        /* for initialize state, we only check memory free space for DN */
        if (IS_PGXC_DATANODE && (uint64)initNeedSize > m_memControl.totalMem)
            ereport(WARNING,
                (errmodule(MOD_VEC_EXECUTOR),
                    errmsg("[VecSonicHashAgg(%d)]:"
                           "The minimum memory needed is %ld, but only have %lu.",
                        m_runtime->ss.ps.plan->plan_node_id,
                        initNeedSize,
                        m_memControl.totalMem)));
    }

    /*
     * based on eqfunction and hashfunc initialized in ExecInitVecAggregate, we need to transfer
     * all these information to SonicHash structure.
     */
    m_equalFuncs = m_runtime->eqfunctions;
    m_buildOp.hashFmgr = m_runtime->hashfunctions;

    m_buildOp.hashFunc = (hashValFun*)palloc(sizeof(hashValFun) * m_buildOp.keyNum);
    initHashFunc(m_buildOp.tupleDesc, (void*)m_buildOp.hashFunc, m_buildOp.keyIndx, false);

    /* initialize hash match function */
    initMatchFunc(m_buildOp.tupleDesc, m_buildOp.keyIndx, m_buildOp.keyNum);

    /* binding build function */
    BindingFp();

    if (m_runtime->ss.ps.instrument) {
        m_runtime->ss.ps.instrument->sorthashinfo.hashtable_expand_times = 0;
    }

    /* We will set this values, if it is only group or plain agg */
    if (m_finalAggNum > 0) {
        m_buildScanBatch = &SonicHashAgg::BuildScanBatchFinal;
    } else {
        m_buildScanBatch = &SonicHashAgg::BuildScanBatchSimple;
    }
}

/*
 * @Description	: Initalize aggregation information.
 * @return		: Initialization of element in SonicHashAgg.
 */
void SonicHashAgg::initAggInfo()
{
    int i = 0;
    ListCell* lc = NULL;
    VecAgg* node = (VecAgg*)(m_runtime->ss.ps.plan);

    /* number of agg funcs and columns needed in hash table */
    m_aggNum = m_runtime->numaggs;
    m_hashNeed = list_length(m_runtime->hash_needed);

    /* prepare build side operator information */
    m_buildOp.keyNum = node->numCols;
    m_buildOp.cols = m_hashNeed;
    m_buildOp.tupleDesc = outerPlanState(m_runtime)->ps_ResultTupleSlot->tts_tupleDescriptor;

    /* grouping column num */
    if (m_buildOp.keyNum > 0) {
        /* key in input data */
        m_buildOp.keyIndx = (uint16*)palloc(sizeof(uint16) * m_buildOp.keyNum);
        for (i = 0; i < m_buildOp.keyNum; i++) {
            m_buildOp.keyIndx[i] = node->grpColIdx[i] - 1;
        }
    }

    /*
     * The hash_needed include all var indexs that be needed in agg operator.
     */
    if (m_hashNeed > 0) {
        m_hashInBatchIdx = (uint16*)palloc(sizeof(uint16) * m_hashNeed);

        /*
         * m_hashInBatchIdx mapping position of outerbatch column in sonic datum array.
         * for example, m_hashInBatchIdx[0] = 3, means batch's 3th column keep in 0th
         * column of sonic datam array.
         */
        i = 0;
        foreach (lc, m_runtime->hash_needed) {
            int varNumber = lfirst_int(lc) - 1;
            m_hashInBatchIdx[i] = varNumber;
            i++;
        }
    }

    if (m_buildOp.keyNum > 0) {
        /* keep indexes of grouping columns in sonic array */
        m_keyIdxInSonic = (uint16*)palloc(sizeof(uint16) * m_buildOp.keyNum);

        for (i = 0; i < m_buildOp.keyNum; i++) {
            bool isFound = false;
            for (int k = 0; k < m_hashNeed; k++) {
                if (m_buildOp.keyIndx[i] == m_hashInBatchIdx[k]) {
                    m_keyIdxInSonic[i] = k;
                    isFound = true;
                    break;
                }
            }
            Assert(isFound);
        }
    }

    /* initialize agg function information */
    if (m_aggNum > 0) {
        m_aggIdx = (uint16*)palloc0(sizeof(uint16) * m_aggNum);
        m_aggCount = (bool*)palloc0(sizeof(bool) * m_aggNum);
        m_finalAggInfo = (finalAggInfo*)palloc0(sizeof(finalAggInfo) * m_aggNum);
    }

    m_finalAggNum = 0;
    int aggIdx = m_buildOp.cols;
    for (int j = 0; j < m_aggNum; j++) {
        m_buildOp.cols++;
        m_aggIdx[j] = aggIdx;

        /* mark count(col), count(*) */
        Oid aggFuncOid = m_runtime->aggInfo[j].vec_agg_function.flinfo->fn_oid;
        if (aggFuncOid == F_INT8INC || aggFuncOid == F_INT8INC_ANY) {
            m_aggCount[j] = true;
        }

        if (m_runtime->aggInfo[j].vec_final_function.flinfo != NULL) {
            m_finalAggInfo[m_finalAggNum].idx = aggIdx;
            m_finalAggInfo[m_finalAggNum].info = &m_runtime->aggInfo[j];
            m_finalAggNum++;

            /*
             * For avg function, we need to store count, sum(x).
             */
            aggIdx += 2;
            m_buildOp.cols += 1;
        } else {
            aggIdx++;
        }
    }
}

/*
 * @Description	: Initialize memorycontrol information.
 */
void SonicHashAgg::initMemoryControl()
{
    VecAgg* vec_agg = (VecAgg*)(m_runtime->ss.ps.plan);
    m_memControl.totalMem = SET_NODEMEM(vec_agg->plan.operatorMemKB[0], vec_agg->plan.dop) * 1024L;
    m_memControl.totalMem += GetAvailRackMemory(vec_agg->plan.dop) * 1024L;

    /* set initial availMem which is passed from optimizer */
    m_memControl.availMem = 0;

    if (vec_agg->plan.operatorMaxMem > vec_agg->plan.operatorMemKB[0]) {
        m_memControl.maxMem = SET_NODEMEM(vec_agg->plan.operatorMaxMem, vec_agg->plan.dop) * 1024L;
    }

    MEMCTL_LOG(DEBUG2,
        "[VecSonicHashAgg(%d)]: Initial total memory: %lu, max memory: %lu.",
        m_runtime->ss.ps.plan->plan_node_id,
        m_memControl.totalMem,
        m_memControl.maxMem);

    m_memControl.sysBusy = false;
    m_memControl.spillToDisk = false;
    m_memControl.spillNum = 0;
    m_memControl.spreadNum = 0;
}

/*
 * @Description	: Build batch information which will be used during sonic hashagg routine
 */
void SonicHashAgg::initBatch()
{
    ListCell* l = NULL;
    ScalarDesc* type_arr = (ScalarDesc*)palloc(sizeof(ScalarDesc) * (m_hashNeed + m_aggNum));
    TupleDesc outDesc = outerPlanState(m_runtime)->ps_ResultTupleSlot->tts_tupleDescriptor;

    /* initialize the type information of all the outer targetlist that we needed */
    for (int i = 0; i < m_hashNeed; i++) {
        type_arr[i].typeId = outDesc->attrs[m_hashInBatchIdx[i]].atttypid;
        type_arr[i].typeMod = outDesc->attrs[m_hashInBatchIdx[i]].atttypmod;
        type_arr[i].encoded = COL_IS_ENCODE(type_arr[i].typeId);
        if (type_arr[i].encoded) {
            m_keySimple = false;
        }
    }

    int idx = m_hashNeed + m_aggNum - 1;
    /* description order to loop over agg functions */
    foreach (l, m_runtime->aggs) {
        AggrefExprState* aggrefstate = (AggrefExprState*)lfirst(l);
        Aggref* aggref = (Aggref*)aggrefstate->xprstate.expr;

        type_arr[idx].typeId = aggref->aggtype;
        type_arr[idx].typeMod = -1;
        type_arr[idx].encoded = COL_IS_ENCODE(type_arr[idx].typeId);
        idx--;
    }

    m_scanBatch = New(CurrentMemoryContext) VectorBatch(CurrentMemoryContext, type_arr, m_hashNeed + m_aggNum);
    m_proBatch = New(CurrentMemoryContext) VectorBatch(CurrentMemoryContext, outDesc);
    m_outBatch = New(CurrentMemoryContext) VectorBatch(CurrentMemoryContext, outDesc);
}

/*
 * @Description	: Initialize sonic data array to represent batch information.
 */
void SonicHashAgg::initDataArray()
{
    int count = 0;
    AutoContextSwitch memSwitch(m_memControl.hashContext);

    /* init storage struct for data */
    DatumDesc desc;

    /* Each column correspond to a SonicDatumArray */
    m_data = (SonicDatumArray**)palloc(sizeof(SonicDatumArray*) * m_buildOp.cols);

    /*
     * Consider hash key type to initialize sonic data array, except integer type, all the other
     * hash type only store the address here.
     */
    for (int i = 0; i < m_hashNeed; i++) {
        Form_pg_attribute attrs = &m_buildOp.tupleDesc->attrs[m_hashInBatchIdx[i]];
        getDataDesc(&desc, 0, attrs, true);

        if (!COL_IS_ENCODE(attrs->atttypid)) {
            m_data[i] = AllocateIntArray(m_memControl.hashContext, m_memControl.hashContext, m_atomSize, true, &desc);
        } else {
            m_data[i] = New(m_memControl.hashContext)
                SonicStackEncodingDatumArray(m_memControl.hashContext, m_atomSize, true, &desc);
        }

        /* to make sure data type is ENCODE_VAR_TYPE */
        if (COL_IS_ENCODE(attrs->atttypid)) {
            m_data[i]->m_desc.dataType = SONIC_VAR_TYPE;
            m_data[i]->m_atomTypeSize = m_data[i]->m_desc.typeSize = sizeof(Datum);
        }

        if (m_data[i]->m_desc.dataType == SONIC_CHAR_DIC_TYPE) {
            /* for SonicCharDatumArray, better to include the typeSize */
            m_arrayElementSize += m_data[i]->m_desc.typeSize;
        } else if (m_data[i]->m_desc.dataType == SONIC_NUMERIC_COMPRESS_TYPE) {
            /* for SonicNumericDatumArray,
             * only inlcude the space for SonicNumericDatumArray->m_curOffset here,
             */
            m_arrayElementSize += (sizeof(uint32) + m_data[i]->m_desc.typeSize);
        } else {
            m_arrayElementSize += m_data[i]->m_atomTypeSize;
        }
        /* consider data flag size : one byte */
        m_arrayElementSize += 1;
        m_arrayExpandSize += (m_data[i]->m_atomTypeSize * (int64)m_atomSize + m_atomSize);

        count++;
    }

    /*
     * Though we consider all the types here, we only support int, bigint, numeric data type in
     * agg function.
     */
    ListCell* lc = NULL;
    Form_pg_attribute attr = (Form_pg_attribute)palloc(sizeof(FormData_pg_attribute));

    int j = m_aggNum;
    int i = m_finalAggNum - 1;
    foreach (lc, m_runtime->aggs) {
        count = m_aggIdx[--j];
        AggrefExprState* aggrefstate = (AggrefExprState*)lfirst(lc);
        Aggref* aggref = (Aggref*)aggrefstate->xprstate.expr;

        /*
         * For INT2 and INT4 agg function, their transition types can be int8 or int8array,
         * here we use int8 to store these two data types.
         * For INT8 and NUMERIC agg function, their transition types are numeric.
         */
        switch (aggref->aggfnoid) {
            case INT2SMALLERFUNCOID:
            case INT2LARGERFUNCOID:
            case INT4SMALLERFUNCOID:
            case INT4LARGERFUNCOID:
            case INT8SMALLERFUNCOID:
            case INT8LARGERFUNCOID:
                attr->atttypid = aggref->aggtrantype;
                getDataDesc(&desc, 0, attr, false);
                m_data[count] =
                    AllocateIntArray(m_memControl.hashContext, m_memControl.hashContext, m_atomSize, true, &desc);
                break;
            case INT2AVGFUNCOID:
            case INT2SUMFUNCOID:
            case INT4AVGFUNCOID:
            case INT4SUMFUNCOID:
            case COUNTOID:
            case ANYCOUNTOID:
                attr->atttypid = INT8OID;
                getDataDesc(&desc, 0, attr, false);
                m_data[count] = New(m_memControl.hashContext)
                    SonicIntTemplateDatumArray<uint64>(m_memControl.hashContext, m_atomSize, true, &desc);
                break;
            default:
                attr->atttypid = aggref->aggtrantype;
                getDataDesc(&desc, 0, attr, false);
                m_data[count] = New(m_memControl.hashContext)
                    SonicEncodingDatumArray(m_memControl.hashContext, m_atomSize, true, &desc);
                break;
        }
        m_arrayElementSize += m_data[count]->m_atomTypeSize;
        m_arrayExpandSize += (m_data[count]->m_atomTypeSize * (int64)m_atomSize + m_atomSize);

        /* Initialize data array for count column */
        if (count == m_finalAggInfo[i].idx) {
            attr->atttypid = INT8OID;
            getDataDesc(&desc, 0, attr, false);
            m_data[count + 1] = New(m_memControl.hashContext)
                SonicIntTemplateDatumArray<uint64>(m_memControl.hashContext, m_atomSize, true, &desc);
            /* For count column, the atom type size if fixed with 8 */
            m_arrayElementSize += 8;
            m_arrayExpandSize += (8 * (int64)m_atomSize + m_atomSize);
            i--;
        }
    }
}

/*
 * @Description	: Initialize Sonic Hash Table.
 */
void SonicHashAgg::initHashTable()
{
    AutoContextSwitch memSwitch(m_memControl.hashContext);
    DatumDesc desc;

    /* initial hash table to restore hash val */
    getDataDesc(&desc, 4, NULL, false);
    m_hash = New(m_memControl.hashContext)
        SonicIntTemplateDatumArray<uint32>(m_memControl.hashContext, m_atomSize, false, &desc);

    /* hash header : first need to decide use segment hash table or not */
    m_useSegHashTbl = (uint64)(sizeof(uint32) * m_hashSize) >= (uint64)MaxAllocSize;
    // size_t total_memory = 0;
    if (m_useSegHashTbl) {
        // total_memory += sizeof(uint32) * m_hashSize;  // 分段哈希表的总大小
        /* how many segment we need, each segment is one atom */
        m_segBucket = New(m_memControl.hashContext)
            SonicIntTemplateDatumArray<uint32>(m_memControl.hashContext, m_atomSize, false, &desc);
        m_segBucket->m_atomIdx = 0;
        m_segNum = (m_hashSize - 1) / INIT_DATUM_ARRAY_SIZE + 1;

        for (int i = 0; i < m_segNum; i++)
            m_segBucket->genNewArray(false);
    } else {
        // total_memory += sizeof(uint32) * m_hashSize;  // 普通哈希桶
        m_bucket = (char*)palloc0(sizeof(uint32) * m_hashSize);
    }

    /* m_atomSize is the initial length of data array with fix length (16*1024) */
    m_next = New(m_memControl.hashContext)
        SonicIntTemplateDatumArray<uint32>(m_memControl.hashContext, m_atomSize, false, &desc);

    /* Set the first value of next array to 0 and this value will not be used */
    errno_t rc =
        memset_s(m_next->m_curAtom->data, m_next->m_atomSize * m_next->m_atomTypeSize, 0, m_next->m_atomTypeSize);
    securec_check(rc, "", "");
    // 2. m_hash数组内存
    // total_memory += m_atomSize * sizeof(uint32);
    // 3. m_next数组内存  
    // total_memory += m_atomSize * sizeof(uint32);
    
    // 4. 其他固定开销估算
    // total_memory += sizeof(uint32) * m_atomSize;  // 预留其他数据结构
    
    // double memory_mb = (double)total_memory / (1024 * 1024);
    
    // printf("[HashTable Init] Size: %u, Memory: %.2f MB, Segmented: %s\n",
    //        m_hashSize, memory_mb, m_useSegHashTbl ? "Yes" : "No");
    // printf("[Memory Pre-alloc] Buckets: %.2f MB, HashArray: %.2f MB, NextArray: %.2f MB\n",
    //        (double)(sizeof(uint32) * m_hashSize) / (1024 * 1024),
    //        (double)(m_atomSize * sizeof(uint32)) / (1024 * 1024),
    //        (double)(m_atomSize * sizeof(uint32)) / (1024 * 1024));
    /*
     * when we have a new atom for keyvalue(m_data), we also create new atom for hashval(m_hash)
     * and position (m_next).
     */
    m_arrayExpandSize += (sizeof(uint32) * m_atomSize + sizeof(uint32) * m_atomSize);
}

/*
 * @Description	: Initialize hash match function for datum array and value.
 * @in desc		: Tuple descriptor used to describe data type info.
 * @in keyIdx		: The serial number of hash key in m_data.
 * @in keyNum	: Number of hash keys.
 */
void SonicHashAgg::initMatchFunc(TupleDesc desc, uint16* keyIdx, uint16 keyNum)
{
    m_arrayKeyMatch = (pKeyMatchArrayFunc*)palloc0(sizeof(pKeyMatchArrayFunc) * keyNum);
    m_valueKeyMatch = (pKeyMatchValueFunc*)palloc0(sizeof(pKeyMatchValueFunc) * keyNum);

    for (int i = 0; i < keyNum; i++) {
        if (integerType(m_data[m_keyIdxInSonic[i]]->m_desc.typeId)) {
            m_arrayKeyMatch[i] = &SonicHashAgg::matchArray<true>;
            m_valueKeyMatch[i] = &SonicHashAgg::matchValue<true>;
        } else {
            m_arrayKeyMatch[i] = &SonicHashAgg::matchArray<false>;
            m_valueKeyMatch[i] = &SonicHashAgg::matchValue<false>;
        }
    }
}

/*
 * @Description	: Binding build function based on segment info.
 */
void SonicHashAgg::BindingFp()
{
    if (m_useSegHashTbl) {
        if (((Agg *) m_runtime->ss.ps.plan)->unique_check) {
            m_buildFun = &SonicHashAgg::buildAggTblBatch<true, true>;
        } else {
            m_buildFun = &SonicHashAgg::buildAggTblBatch<true, false>;
        }
    } else {
        if (((Agg *) m_runtime->ss.ps.plan)->unique_check) {
            m_buildFun = &SonicHashAgg::buildAggTblBatch<false, true>;
        } else {
            m_buildFun = &SonicHashAgg::buildAggTblBatch<false, false>;
        }
    }
}

/*
 * @Description	: sonic hash agg reset function.
 * @in node		: vector aggregation state node.
 * @return		: return true if reset operation is done.
 */
bool SonicHashAgg::ResetNecessary(VecAggState* node)
{
    m_stateLog.restore = false;
    m_stateLog.lastProcessIdx = 0;

    VecAgg* agg_node = (VecAgg*)node->ss.ps.plan;

    /*
     * If we do have the hash table, and the subplan does not have any
     * parameter changes, and none of our own parameter changes affect
     * input expressions of the aggregated functions, then we can just
     * rescan the existing hash table, and have not spill to disk;
     * no need to build it again.
     */
    if (m_memControl.spillToDisk == false && node->ss.ps.lefttree->chgParam == NULL && agg_node->aggParams == NULL) {
        m_runState = AGG_FETCH;
        return false;
    }

    /* release partition resouce if spill is true */
    if (m_memControl.spillToDisk) {
        SonicHashAgg* vsonichagg = (SonicHashAgg*)node->aggRun;
        if (vsonichagg && m_partFileSource) {
            for (int i = 0; i < m_partNum; i++) {
                if (m_partFileSource[i]) {
                    m_partFileSource[i]->freeResources();
                }
            }
        }
    }

    m_runState = AGG_PREPARE;

    /* Reset context */
    MemoryContextResetAndDeleteChildren(m_memControl.hashContext);
    /* Rebuild initial hash table */
    {
        AutoContextSwitch memSwitch(m_memControl.hashContext);

        /* reinitialize sonic datum arry */
        m_arrayElementSize = 0;
        m_arrayExpandSize = 0;
        initDataArray();

        int64 hashSize = Min((uint64)agg_node->numGroups * 2, m_memControl.totalMem / m_arrayElementSize);
        m_hashSize = calcHashTableSize<false, false>(hashSize);

        /* reinitialize sonic hash table */
        initHashTable();

        /* reset runtime build function */
        BindingFp();
    }

    m_rows = 0;
    m_fill_table_rows = 0;
    m_partFileSource = NULL;
    m_overflowFileSource = NULL;

    m_memControl.availMem = 0;
    m_memControl.spillToDisk = false;
    m_strategy = HASH_IN_MEMORY;

    return true;
}

/*
 * @Description	: SonicHashAgg entrance function
 */
VectorBatch* SonicHashAgg::Run()
{
    VectorBatch* res = NULL;

    while (true) {
        switch (m_runState) {
            /* Get data source , left-tree or temp file */
            case AGG_PREPARE: {
                m_sonicHashSource = GetHashSource();

                /* if no source, it is the end */
                if (m_sonicHashSource == NULL) {
                    return NULL;
                }
                m_runState = AGG_BUILD;
                break;
            }

            /* Get data and insert into hash table */
            case AGG_BUILD: {
                Build();

                /* print the hash table information when needed */
                bool can_wlm_warning_stats = false;
                if (anls_opt_is_on(ANLS_HASH_CONFLICT)) {
                    char stats[MAX_LOG_LEN];
                    Profile(stats, &can_wlm_warning_stats);

                    if (m_memControl.spillToDisk == false) {
                        ereport(LOG,
                            (errmodule(MOD_VEC_EXECUTOR),
                                errmsg("[VecSonicHashAgg(%d)] %s", m_runtime->ss.ps.plan->plan_node_id, stats)));
                    } else {
                        ereport(LOG,
                            (errmodule(MOD_VEC_EXECUTOR),
                                errmsg("[VecSonicHashAgg(%d)(temp file:%d)] %s",
                                    m_runtime->ss.ps.plan->plan_node_id,
                                    m_currPartIdx,
                                    stats)));
                    }
                } else if (u_sess->attr.attr_resource.resource_track_level >= RESOURCE_TRACK_QUERY &&
                           u_sess->attr.attr_resource.enable_resource_track && u_sess->exec_cxt.need_track_resource) {
                    char stats[MAX_LOG_LEN];
                    Profile(stats, &can_wlm_warning_stats);
                }

                /* mark meet serious hash conflict case */
                if (can_wlm_warning_stats) {
                    pgstat_add_warning_hash_conflict();
                    if (m_runtime->ss.ps.instrument) {
                        m_runtime->ss.ps.instrument->warning |= (1 << WLM_WARN_HASH_CONFLICT);
                    }
                }

                if (!m_memControl.spillToDisk) {
                    /* Early free left tree after hash table built */
                    ExecEarlyFree(outerPlanState(m_runtime));

                    EARLY_FREE_LOG(elog(LOG,
                        "Early Free: Hash Table for SonicHashAgg"
                        " is built at node %d, memory used %d MB.",
                        (m_runtime->ss.ps.plan)->plan_node_id,
                        getSessionMemoryUsageMB()));
                }

                m_runState = AGG_FETCH;
                break;
            }

            /* Fetch data from hash table and return result */
            case AGG_FETCH: {
                // auto probe_start = std::chrono::high_resolution_clock::now();
                res = Probe();
                // auto probe_end = std::chrono::high_resolution_clock::now();
                // auto probe_duration = std::chrono::duration_cast<std::chrono::microseconds>(probe_end - probe_start);
                // double probe_time_ms = probe_duration.count() / 1000.0;
                // printf("hash agg probe time %.3fms\n",probe_time_ms);
                if (BatchIsNull(res)) {
                    /* If not matched, turn to next partition */
                    if (true == m_memControl.spillToDisk) {
                        m_strategy = HASH_IN_DISK;
                        m_runState = AGG_PREPARE;
                    } else {
                        return NULL;
                    }
                } else {
                    return res;
                }
                break;
            }
            default:
                ereport(ERROR,
                    (errcode(ERRCODE_UNDEFINED_OBJECT),
                        errmodule(MOD_VEC_EXECUTOR),
                        (errmsg("Unrecognized vector sonic hashagg run status."))));
                break;
        }
    }

    return NULL;
}

/*
 * @Description: get batch from lefttree or temp file and insert into hash table.
 */
void SonicHashAgg::Build()
{
    VectorBatch* outer_batch = NULL;

    /* load data, build hash table & calculate agg function */
    WaitState oldStatus = pgstat_report_waitstatus(STATE_EXEC_HASHAGG_BUILD_HASH);
    for (;;) {
        outer_batch = m_sonicHashSource->getBatch();
        //u_sess->stream_cxt.trace_tsc_obj->start();
        if (unlikely(BatchIsNull(outer_batch))) {
            break;
        }

        /* first try to expand hash table if needed */
        tryExpandHashTable();

        (this->*m_buildFun)(outer_batch);
        //u_sess->stream_cxt.trace_tsc_obj->stop();
    }
    (void)pgstat_report_waitstatus(oldStatus);

    /* record state of sonic hashagg */
    // if (HAS_INSTR(&m_runtime->ss, false)) {
        if (m_tupleCount > 0) {
            m_runtime->ss.ps.instrument->width = (int)(m_colWidth / m_tupleCount);
        } else {
            m_runtime->ss.ps.instrument->width = (int)m_colWidth;
        }

        m_runtime->ss.ps.instrument->spreadNum = m_memControl.spreadNum;
        m_runtime->ss.ps.instrument->sysBusy = m_memControl.sysBusy;
        m_runtime->ss.ps.instrument->sorthashinfo.hashbuild_time = m_hashbuild_time;
        m_runtime->ss.ps.instrument->sorthashinfo.hashagg_time = m_calcagg_time;


        // int curr_cpu = sched_getcpu();
        // elog(LOG,
        //     "[CPU %d VecSonicHashAgg(%d)] smp %d:  Hash table size is %.4f MB, m_rows :%ld build_time %.3f, agg_time %.3f.",
        //     curr_cpu,
        //     m_runtime->ss.ps.plan->plan_node_id,
        //     // m_hashSize,
        //     u_sess->stream_cxt.smp_id,
        //     // group_positions.size(),
        //     (double)(sizeof(uint32) * m_hashSize) / (1024 * 1024),
        //     m_rows,
        //     m_hashbuild_time,
        //     m_calcagg_time
        //     );
    // }
    // elog(LOG,
    //         "[CPU %d VecSonicHashAgg(%d)] smp %d:  Hash table size is %.4f MB, m_rows :%ld build_time %.3f, agg_time %.3f.",
    //         curr_cpu,
    //         m_runtime->ss.ps.plan->plan_node_id,
    //         // m_hashSize,
    //         u_sess->stream_cxt.smp_id,
    //         // group_positions.size(),
    //         (double)(sizeof(uint32) * m_hashSize) / (1024 * 1024),
    //         m_rows,
    //         m_hashbuild_time,
    //         m_calcagg_time
    //         );
    //elog(LOG,"-----------VecSonicHashAgg(%d)-----------", m_runtime->ss.ps.plan->plan_node_id);
    //u_sess->stream_cxt.trace_tsc_obj->print_stats();
}

/*
 * @Description	: Probe process.
 */
VectorBatch* SonicHashAgg::Probe()
{
    int last_idx = 0;
    VectorBatch* ret = NULL;
    int rows;

    m_scanBatch->Reset();
    ResetExprContext(m_runtime->ss.ps.ps_ExprContext);

    if (m_stateLog.restore) {
        last_idx = m_stateLog.lastProcessIdx;
        m_stateLog.restore = false;
    }

    while (last_idx < m_rows) {
        if (last_idx == m_rows) {
            return NULL;
        }

        rows = (BatchMaxSize < (m_rows - last_idx)) ? BatchMaxSize : (m_rows - last_idx);

        for (int i = 0; i < rows; i++) {
            last_idx++;
            InvokeFp(m_buildScanBatch)(last_idx);
        }

        m_stateLog.lastProcessIdx = last_idx;
        m_stateLog.restore = true;
        ret = ProducerBatch();

        if (BatchIsNull(ret)) {
            m_scanBatch->Reset();
            continue;
        } else {
            break;
        }
    }

    return ret;
}

/*
 * @Description	: get data source. The first is lefttree, or temp file if has write temp file.
 */
SonicHashSource* SonicHashAgg::GetHashSource()
{
    SonicHashSource* ps = NULL;

    switch (m_strategy) {
        case HASH_IN_MEMORY: {
            ps = New(CurrentMemoryContext) SonicHashOpSource(outerPlanState(m_runtime));
            break;
        }

        case HASH_IN_DISK: {
            /* m_spillToDisk is false means there is no data in disk */
            if (!m_memControl.spillToDisk) {
                return NULL;
            }

            /* close last partition files and go to the next */
            if (m_currPartIdx >= 0) {
                m_partFileSource[m_currPartIdx]->freeResources();
                m_partFileSource[m_currPartIdx] = NULL;
            }

            /* find the next valid partition, which means we have to load data */
            m_currPartIdx++;
            while (m_currPartIdx < m_partNum && m_partFileSource[m_currPartIdx]->m_rows == 0) {
                m_partFileSource[m_currPartIdx]->freeResources();
                m_partFileSource[m_currPartIdx] = NULL;
                m_currPartIdx++;
            }

            if (m_currPartIdx < m_partNum) {
                ps = m_partFileSource[m_currPartIdx];
                Assert(m_partFileSource[m_currPartIdx]->m_status == partitionStatusFile);
                ((SonicHashFilePartition*)m_partFileSource[m_currPartIdx])->rewindFiles();

                /* reset the rows in hash table */
                m_rows = 0;

                /* reset context to free the memory */
                MemoryContextResetAndDeleteChildren(m_memControl.hashContext);

                /* recompute hash table size and initialize hash structure, since we have already
                 * do reset in FilePartition for batch, so not need to reset partition context here.
                 */
                {
                    AutoContextSwitch memSwitch(m_memControl.hashContext);

                    int64 hashSize = Min(
                        (uint64)m_partFileSource[m_currPartIdx]->m_rows, m_memControl.availMem / m_arrayElementSize);
                    m_hashSize = calcHashTableSize<false, false>(hashSize);
                    MEMCTL_LOG(DEBUG2,
                        "[VecSonicHashAgg(%d)(temp partition %d)]: "
                        "current partition rows:%ld, new hash table size:%ld,"
                        " availmem:%lu, elemSize:%d.",
                        m_runtime->ss.ps.plan->plan_node_id,
                        m_currPartIdx,
                        m_partFileSource[m_currPartIdx]->m_rows,
                        m_hashSize,
                        m_memControl.availMem,
                        m_arrayElementSize);

                    /* reinitialize sonic data array */
                    m_arrayElementSize = 0;
                    m_arrayExpandSize = 0;
                    initDataArray();

                    /* reinitialize sonic hash table */
                    initHashTable();

                    /* reset runtime build function */
                    BindingFp();
                }

                m_stateLog.restore = false;
                m_stateLog.lastProcessIdx = 0;

                /* get the right strategy */
                m_strategy = HASH_IN_MEMORY;
            } else {
                pfree(m_partFileSource);
                m_partFileSource = NULL;

                if (NULL == m_overflowFileSource) {
                    return NULL;
                } else {
                    /* switch to the overflow hash file source */
                    m_partFileSource = m_overflowFileSource;
                    m_overflowFileSource = NULL;

                    m_partNum = m_overflowNum;

                    /* reset current partition index */
                    m_currPartIdx = -1;
                    m_overflowNum = 0;

                    return GetHashSource();
                }
            }
            break;
        }

        default:
            ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_OBJECT),
                    errmodule(MOD_VEC_EXECUTOR),
                    (errmsg("Unrecognized vector sonic hashagg data status."))));
            break;
    }

    return ps;
}

/*
 * @Description	: Analyze current hash table to show the statistics information of hash chains,
 *				  including hash table size, invalid number of hash chains, distribution of the
 *				  length of hash chains.
 * @in stats		: The string used to record all the hash table information.
 * @in can_wlm_warning_statistics	: flag used to mask
 */
void SonicHashAgg::Profile(char* stats, bool* can_wlm_warning_statistics)
{
    uint32 loc = 0;
    uint32 fill_rows = 0;
    uint32 single_num = 0;
    uint32 double_num = 0;
    uint32 conflict_num = 0;
    uint32 total_num = 0;
    uint32 hash_size = m_hashSize;
    uint32 chain_len = 0;
    uint32 max_chain_len = 0;

    for (uint32 idx = 0; idx < hash_size; idx++) {
        if (m_useSegHashTbl) {
            loc = m_segBucket->getNthDatum(idx);
        } else {
            loc = ((uint32*)m_bucket)[idx];
        }

        if (loc != 0) {
            chain_len = 0;

            while (loc != 0) {
                chain_len++;
                fill_rows++;
                loc = m_next->getNthDatum(loc);
            }

            /* record the number of hash chains with length equal to 1 */
            if (chain_len == 1) {
                single_num++;
            }

            /* record the number of hash chains with length equal to 2 */
            if (chain_len == 2) {
                double_num++;
            }

            /* mark if the length of hash chain is greater than 3, we meet hash confilct */
            if (chain_len >= 3) {
                conflict_num++;
            }

            /* record the length of the max hash chain */
            if (chain_len > max_chain_len) {
                max_chain_len = chain_len;
            }

            if (chain_len != 0) {
                total_num++;
            }
        }
    }

    /* print the information */
    errno_t rc = sprintf_s(stats,
        MAX_LOG_LEN,
        "Sonic HashTable Profiling: table size: %u,"
        " hash elements: %u, table fill ratio %.2f, max hash chain len: %u,"
        " %u chains have length 1, %u chains have length 2, %u chains have conficts"
        " with length >= 3.",
        hash_size,
        fill_rows,
        (double)fill_rows / hash_size,
        max_chain_len,
        single_num,
        double_num,
        conflict_num);
    securec_check_ss(rc, "\0", "\0");

    if (max_chain_len >= WARNING_HASH_CONFLICT_LEN || (total_num != 0 && conflict_num >= total_num / 2)) {
        *can_wlm_warning_statistics = true;
    }
}

/*
 * @Description	: Check hash key match result with respect to special value.
 * @in pVector	: The column we need to check.
 * @in keyIdx		: The serial number of the hash key.
 * @in pVectorIdx	: The serial number of pVector.
 * @in cmpIdx		: The location of hash table.
 * @return		: Return true is matched.
 */
template <bool simpleType>
bool SonicHashAgg::matchValue(ScalarVector* pVector, uint16 keyIdx, int16 pVectorIdx, uint32 cmpIdx)
{
    Datum val;
    uint8 flag;
    bool notnull_check = false;
    bool null_check = false;

    m_data[m_keyIdxInSonic[keyIdx]]->getNthDatumFlag(cmpIdx, &val, &flag);
    notnull_check = BOTH_NOT_NULL(pVector->m_flag[pVectorIdx], flag);
    null_check = BOTH_NULL(pVector->m_flag[pVectorIdx], flag);

    if (simpleType) {
        notnull_check = notnull_check && (pVector->m_vals[pVectorIdx] == val);
    } else {
        FunctionCallInfoData fcinfo;
        Datum args[2];
        fcinfo.arg = &args[0];
        fcinfo.arg[0] = pVector->m_vals[pVectorIdx];
        fcinfo.arg[1] = val;
        fcinfo.flinfo = (m_equalFuncs + keyIdx);
        notnull_check = notnull_check && (bool)m_equalFuncs[keyIdx].fn_addr(&fcinfo);
    }

    // if (cmpIdx % 5000 == 0) {
    // printf("[ProbeMatch][Thread %lu][smpid=%ld] "
    //        "keyIdx=%u cmpIdx=%u val=0x%lx flag=%u pVal=0x%lx pFlag=%u\n",
    //        gs_thread_self(),
    //        u_sess->stream_cxt.smp_id,
    //        keyIdx,
    //        cmpIdx,
    //        val, flag,
    //        pVector->m_vals[pVectorIdx],
    //        pVector->m_flag[pVectorIdx]);
    // }
    // if (cmpIdx % 5000 == 0) {
    // elog(LOG,
    //      "[ProbeMatch][Thread %lu][smpid=%ld] "
    //      "keyIdx=%u cmpIdx=%u val=0x%lx flag=%u pVal=0x%lx pFlag=%u",
    //      gs_thread_self(),
    //      u_sess->stream_cxt.smp_id,
    //      keyIdx,
    //      cmpIdx,
    //      val,
    //      flag,
    //      pVector->m_vals[pVectorIdx],
    //      pVector->m_flag[pVectorIdx]);
    // }


    return (notnull_check || null_check);
}

/*
 * @Description	: Check hash key match result with respect to cmpRows elements in one column.
 * @in pVector	: The column we need to check.
 * @in keyIdx		: The serial number of the hash key.
 * @in cmpRows	: The number of rows we need to consider.
 */
template <bool simpleType>
void SonicHashAgg::matchArray(ScalarVector* pVector, uint16 keyIdx, uint16 cmpRows)
{
    Datum val;
    uint8 flag;
    bool notnull_check = false;
    bool null_check = false;
    Datum args[2];

    FunctionCallInfoData fcinfo;
    fcinfo.arg = &args[0];

    for (int i = 0; i < cmpRows; i++) {
        /* only doing compare when match is true */
        if (m_match[i]) {
            m_data[m_keyIdxInSonic[keyIdx]]->getNthDatumFlag(m_loc[m_suspectIdx[i]], &val, &flag);
            notnull_check = BOTH_NOT_NULL(pVector->m_flag[m_suspectIdx[i]], flag);
            null_check = BOTH_NULL(pVector->m_flag[m_suspectIdx[i]], flag);

            if (simpleType) {
                notnull_check = notnull_check && (pVector->m_vals[m_suspectIdx[i]] == val);
                m_match[i] = notnull_check || null_check;
            } else {
                fcinfo.arg[0] = pVector->m_vals[m_suspectIdx[i]];
                fcinfo.arg[1] = val;
                fcinfo.flinfo = (m_equalFuncs + keyIdx);
                m_match[i] = null_check || (notnull_check && (bool)m_equalFuncs[keyIdx].fn_addr(&fcinfo));
            }
        }
    }
}

/*
 * @Description	: Get data batch and build hash table.
 * @in batch		: data source from lefttree or temp file
 */
template <bool useSegHashTable, bool unique_check>
void SonicHashAgg::buildAggTblBatch(VectorBatch* batch)
{
    int i, j;
    int rows = batch->m_rows;
    instr_time start_time;
    int krows = 0;
    int miss_idx = 0;
    errno_t rc = 0;

    uint32* hash_val = NULL;
    uint32 hash_loc;
    uint32 data_loc;
    uint32 current_loc;
    // ============ 缓存性能测试开始 ============
    // static size_t total_accesses = 0;
    // static size_t fast_accesses = 0;  // 缓存命中
    // static size_t total_batches = 0;
    // instr_time cache_test_start;
    // static std::vector<size_t> bucket_access_count(1024); // 桶访问计数
    // static size_t total_hot_bucket_accesses{0};
    // INSTR_TIME_SET_CURRENT(cache_test_start);
    // std::set<uint32_t> group_positions;
    // u_sess->stream_cxt.trace_cache_obj->start();
    INSTR_TIME_SET_CURRENT(start_time);


    /* the hash table may be resized yet */
#ifdef USE_PRIME
    uint32 mask = m_hashSize;
#else
    uint32 mask = m_hashSize - 1;
#endif

    /* 1. calculate hash value. */
    hashBatchArray(batch, (void*)m_buildOp.hashFunc, m_buildOp.hashFmgr, m_buildOp.keyIndx, m_hashVal);
    hash_val = m_hashVal;

    m_missNum = 0;
    m_suspectNum = 0;

    /* seperate tuples that missed and matched */
    for (i = 0; i < rows; i++) {
#ifdef USE_PRIME
        hash_loc = hash_val[i] % mask;
#else
        hash_loc = hash_val[i] & mask;
#endif
        // ============ 单个内存访问计时 ============
        // instr_time access_start;
        // INSTR_TIME_SET_CURRENT(access_start);

        if (!useSegHashTable) {
            data_loc = ((uint32*)m_bucket)[hash_loc];
        } else {
            data_loc = (uint32)m_segBucket->getNthDatum(hash_loc);
        }

        if (!data_loc) {
            m_missIdx[m_missNum++] = i;
        } else {
            m_suspectIdx[m_suspectNum++] = i;
        }

        m_bucketLoc[i] = hash_loc;
        m_loc[i] = data_loc;
        m_orgLoc[i] = data_loc;
        // if(m_loc[i] !=0 ){
        //     group_positions.insert(m_loc[i]);
        // }
        // total_accesses++;
        // // 假设访问时间 < 100纳秒为缓存命中，> 500纳秒为缓存缺失
        // if (access_time < 0.03) {
        //     fast_accesses++;
        // }
        // size_t current_count = ++bucket_access_count[hash_loc];
        // if (current_count > 100) { // 热点桶阈值
        //     total_hot_bucket_accesses++;
        // }
        // 每10个批次输出一次缓存统计
       

    }
    
    // u_sess->stream_cxt.trace_cache_obj->print_stats();
    // double access_time = elapsed_time(&access_start) * 1e6; // 微秒
    // u_sess->stream_cxt.ht_access+=access_time;
    // u_sess->stream_cxt.tt_rows+=rows;
    // double batch_cache_time = elapsed_time(&cache_test_start) * 1000; // 毫秒
    // double cache_hit_rate = (double)fast_accesses / total_accesses * 100;
    // double avg_access_time = (batch_cache_time * 1000) / rows; // 微秒/访问
    // size_t hot_buckets = 0;
    // size_t total_active_buckets = 0;
    // size_t max_bucket_accesses = 0;
    
    // for (size_t i = 0; i < 1024; i++) {
    //     if (bucket_access_count[i] > 0) {
    //         total_active_buckets++;
    //         if (bucket_access_count[i] > rows / 10) { // 热点桶定义
    //             hot_buckets++;
    //         }
    //         if (bucket_access_count[i] > max_bucket_accesses) {
    //             max_bucket_accesses = bucket_access_count[i];
    //         }
    //     }
    // }
    //  double hot_bucket_ratio = total_active_buckets > 0 ? 
    //     (double)hot_buckets / total_active_buckets * 100 : 0;
    //     elog(LOG, "Access Distribution: ActiveBuckets=%zu, HotBuckets=%zu(%.1f%%)", 
    //      total_active_buckets, hot_buckets, hot_bucket_ratio);
    //     elog(LOG, "Cache Stats: HitRate=%.1f%%, AvgAccess=%.3fus, Accesses=%zu", 
    //         cache_hit_rate, avg_access_time, total_accesses);
        
    //     // 根据性能特征给出诊断
    //     if (cache_hit_rate < 80.0) {
    //         elog(LOG, " 🔴 POOR CACHE PERFORMANCE: High cache miss rate");
    //     } else if (cache_hit_rate > 95.0) {
    //         elog(LOG, " ✅ EXCELLENT CACHE PERFORMANCE");
    //     }
    /* m_loc record the final right agg location. this must be a convergence process */
    int matched = 0;
    while (m_suspectNum != 0) {
        krows = 0;

        rc = memset_s(m_match, BatchMaxSize * sizeof(bool), true, m_suspectNum * sizeof(bool));
        securec_check(rc, "", "");
        // // ============ 冲突解析缓存测试 ============
        // instr_time conflict_start;
        // INSTR_TIME_SET_CURRENT(conflict_start);
        /* check hash match */
        for (j = 0; j < m_buildOp.keyNum; j++) {
            RuntimeBinding(m_arrayKeyMatch, j)(&batch->m_arr[m_buildOp.keyIndx[j]], j, m_suspectNum);
        }
        // double conflict_time = elapsed_time(&conflict_start) * 1000; // 毫秒

        /* refresh the next loc */
        for (i = 0; i < m_suspectNum; i++) {
            if (!m_match[i]) {
                miss_idx = m_suspectIdx[i];
                data_loc = m_next->getNthDatum(m_loc[miss_idx]);

                //             // ============ 链表遍历缓存测试 ============
                // instr_time list_start;
                // INSTR_TIME_SET_CURRENT(list_start);
                
                // double list_access_time = elapsed_time(&list_start) * 1e6; // 微秒
                // if (list_access_time > 1.0) { // 链表访问较慢
                //     elog(LOG, "Slow list access: %.3f us at loc %u", list_access_time, m_loc[miss_idx]);
                // }

                if (data_loc) {
                    m_suspectIdx[krows++] = miss_idx;

                    /* update the new position */
                    m_loc[miss_idx] = data_loc;
                } else
                    m_missIdx[m_missNum++] = miss_idx;
            } else {
                if (unique_check) {
                    ereport(ERROR,
                            (errcode(ERRCODE_CARDINALITY_VIOLATION),
                             errmsg("more than one row returned by a subquery used as an expression")));
                }
                matched++;
            }
        }

        m_suspectNum = krows;
    }

    Assert(m_missNum + matched == rows);

    /* last we got the miss list, we must insert into the hashtable. */
    bool keymatch = true;
    uint16 cmpBatchIdx = 0;
    for (i = 0; i < m_missNum; i++) {
        /* the batch element need to handle */
        cmpBatchIdx = m_missIdx[i];

        /* The actual value pos recorded in hash table */
        if (!useSegHashTable) {
            current_loc = ((uint32*)m_bucket)[m_bucketLoc[cmpBatchIdx]];
        } else {
            current_loc = (uint32)m_segBucket->getNthDatum(m_bucketLoc[cmpBatchIdx]);
        }

        /* handle duplicate : follow the list until we found the loc we record at the beginning.
         * the bucket next is not changed, so we can direct insert.
         */
        keymatch = false;
        while (m_orgLoc[cmpBatchIdx] != current_loc && current_loc != 0) {
            /* the bucket has changed ,so we must check match status. */
            for (j = 0; j < m_buildOp.keyNum; j++) {
                keymatch =
                    RuntimeBinding(m_valueKeyMatch, j)(&batch->m_arr[m_buildOp.keyIndx[j]], j, cmpBatchIdx, current_loc);
                if (!keymatch) {
                    break;
                }
            }

            if (keymatch) {
                m_loc[cmpBatchIdx] = current_loc;
                break;
            } else {
                current_loc = m_next->getNthDatum(current_loc);
            }
        }

        /* if not matched, it is a new hashvalue */
        if (!keymatch) {
            AllocHashTbl(batch, m_missIdx[i], hash_val[m_missIdx[i]], m_bucketLoc[cmpBatchIdx]);
        } else if (unique_check) {
            ereport(ERROR,
                    (errcode(ERRCODE_CARDINALITY_VIOLATION),
                     errmsg("more than one row returned by a subquery used as an expression")));
        }
    }

    m_hashbuild_time += elapsed_time(&start_time);

    INSTR_TIME_SET_CURRENT(start_time);
    // u_sess->stream_cxt.trace_cache_obj->stop();
    if (m_runtime->jitted_sonicbatchagg) {
        if (HAS_INSTR(&m_runtime->ss, false)) {
            m_runtime->ss.ps.instrument->isLlvmOpt = true;
        }

        typedef void (*vsonicbatchagg_func)(SonicHashAgg* sonicagg, VectorBatch* batch, uint16* aggIdx);
        ((vsonicbatchagg_func)(m_runtime->jitted_sonicbatchagg))(this, batch, m_aggIdx);
    } else {
        BatchAggregation(batch);
        // ===================== 集成 probe 阶段全局分析 =====================
        // if (batches_processed % 10 == 0 || batches_processed == 1) {
        //     analyze_probe_scalability(m_parallel_degree);  // 多线程性能分析
        //     analyze_q1_specific_bottlenecks();            // 查询特定分析
        // }
        // ==============================================================
    }
    m_calcagg_time += elapsed_time(&start_time);
    
    // if(m_runtime->ss.ps.plan->plan_node_id == 6){
            // elog(LOG,
            // "[CPU %d VecSonicHashAgg(%d)] smp %d: groups %d Hash table size is %.4f MB, m_rows :%ld.",
            // curr_cpu,
            // m_runtime->ss.ps.plan->plan_node_id,
            // // m_hashSize,
            // u_sess->stream_cxt.smp_id,
            // // group_positions.size(),
            // (double)(sizeof(uint32) * m_hashSize) / (1024 * 1024),
            // m_rows
            // );
    // }

}

/*
 * @Description	: put the idx-th row of batch into hash table.
 * @in idx		: The row number of the data.
 * @in hashLoc	: The position of m_bucket.
 * @return		:
 */
// int64 SonicHashAgg::insertHashTbl(VectorBatch* batch, int idx, uint32 hashval, uint32 hashLoc)
// {
//     // === Add this for debugging ===
//     printf("[Debug] Thread %lu: SonicHashAgg=%p, m_bucket=%p, m_segBucket=%p, m_rows=%ld\n",
//            gs_thread_self(),
//            this,
//            m_bucket,
//            m_segBucket,
//            m_rows);
//     fflush(stdout);
//     // ==============================
//     int i;
//     int64 extra_size_needed = 0;
//     ScalarVector* scalar_vec = NULL;

//     m_rows++;

//     /* input hash key value */
//     for (i = 0; i < m_hashNeed; i++) {
//         scalar_vec = &batch->m_arr[m_hashInBatchIdx[i]];
//         m_data[i]->putArray(&scalar_vec->m_vals[idx], &scalar_vec->m_flag[idx], 1);

//         if (likely(NOT_NULL(scalar_vec->m_flag[idx]))) {
//             if (m_tupleCount >= 0 && scalar_vec->m_desc.encoded) {
//                 extra_size_needed += VARSIZE_ANY(scalar_vec->m_vals[idx]);
//             }
//         }
//     }
//     m_colWidth += extra_size_needed;

//     /* set the init agg value */
//     for (i = 0; i < m_aggNum; i++) {
//         uint64 init_val = 0;
//         uint8 init_flag = V_NULL_MASK;
//         m_data[m_aggIdx[i]]->putArray(&init_val, &init_flag, 1);

//         if (m_runtime->aggInfo[i].vec_final_function.flinfo != NULL) {
//             m_data[m_aggIdx[i] + 1]->putArray(&init_val, &init_flag, 1);
//         }
//     }

//     /* restore hashval */
//     Datum tmp_hashval = UInt32GetDatum(hashval);
//     m_hash->putArray((ScalarValue*)&tmp_hashval, NULL, 1);

//     /* update hash table */
//     if (likely(!m_useSegHashTbl)) {
//         m_next->putArray((Datum*)&(((uint32*)m_bucket)[hashLoc]), NULL, 1);
//         ((uint32*)m_bucket)[hashLoc] = m_rows;
//     } else {
//         uint32 bucket_pos = (uint32)m_segBucket->getNthDatum(hashLoc);
//         m_next->putArray((Datum*)&(bucket_pos), NULL, 1);
//         m_segBucket->setNthDatum(hashLoc, (ScalarValue*)&m_rows);
//     }

//     m_loc[idx] = m_rows;

//     return extra_size_needed;
// }
// int64 SonicHashAgg::insertHashTbl(VectorBatch* batch, int idx, uint32 hashval, uint32 hashLoc)
// {
//     int i;
//     int64 extra_size_needed = 0;
//     ScalarVector* scalar_vec = NULL;

//     // -----------------------------
//     // 每 1000 行输出一次日志，打印线程和 SMP ID
//     // -----------------------------
//     if ((m_rows % 1000) == 0) {
//         printf("[SonicHashAgg] Thread %lu, smpid=%ld, inserting row %ld\n",
//                gs_thread_self(), u_sess->stream_cxt.smpidint64, m_rows);
//     }

//     // 打印基础信息
//     printf("\n[Debug] Thread %lu\n", gs_thread_self());
//     printf("  SonicHashAgg=%p, m_bucket=%p, m_segBucket=%p, m_rows(before)=%ld\n",
//            this, m_bucket, m_segBucket, m_rows + 1);
//     printf("  hashval = %u | hashLoc = %u\n", hashval, hashLoc);

//     if (!m_useSegHashTbl) {
//         printf("  [Before] bucket[%u] = %u\n", hashLoc, ((uint32*)m_bucket)[hashLoc]);
//     } else {
//         printf("  [Before] segBucket[%u] = %u\n", hashLoc, (uint32)m_segBucket->getNthDatum(hashLoc));
//     }

//     m_rows++;

//     /* 输入哈希键 */
//     for (i = 0; i < m_hashNeed; i++) {
//         scalar_vec = &batch->m_arr[m_hashInBatchIdx[i]];
//         ScalarValue keyVal = scalar_vec->m_vals[idx];
//         uint8 keyFlag = scalar_vec->m_flag[idx];
//         m_data[i]->putArray(&keyVal, &keyFlag, 1);

//         if (likely(NOT_NULL(keyFlag))) {
//             if (m_tupleCount >= 0 && scalar_vec->m_desc.encoded) {
//                 extra_size_needed += VARSIZE_ANY(keyVal);
//             }
//         }

//         // 打印 key 值
//         if (IS_NULL(keyFlag)) {
//             printf("  key[%d] = NULL\n", i);
//         } else {
//             switch (scalar_vec->m_desc.typeId) {
//                 case INT4OID:
//                     printf("  key[%d] (INT4) = %d\n", i, DatumGetInt32(keyVal));
//                     break;
//                 case INT8OID:
//                     printf("  key[%d] (INT8) = %ld\n", i, DatumGetInt64(keyVal));
//                     break;
//                 default:
//                     printf("  key[%d] (type=%u, raw=0x%lx)\n", i, scalar_vec->m_desc.typeId, keyVal);
//                     break;
//             }
//         }
//     }

//     m_colWidth += extra_size_needed;

//     /* 初始化聚合值 */
//     for (i = 0; i < m_aggNum; i++) {
//         uint64 init_val = 0;
//         uint8 init_flag = V_NULL_MASK;
//         m_data[m_aggIdx[i]]->putArray(&init_val, &init_flag, 1);

//         if (m_runtime->aggInfo[i].vec_final_function.flinfo != NULL) {
//             m_data[m_aggIdx[i] + 1]->putArray(&init_val, &init_flag, 1);
//         }
//     }

//     /* 存储哈希值 */
//     Datum tmp_hashval = UInt32GetDatum(hashval);
//     m_hash->putArray((ScalarValue*)&tmp_hashval, NULL, 1);

//     /* 更新哈希表 */
//     if (likely(!m_useSegHashTbl)) {
//         uint32 old_head = ((uint32*)m_bucket)[hashLoc];
//         m_next->putArray((Datum*)&old_head, NULL, 1);
//         ((uint32*)m_bucket)[hashLoc] = m_rows;
//         printf("  [After] bucket[%u]: old_head=%u -> new_head=%ld\n", hashLoc, old_head, m_rows);
//     } else {
//         uint32 old_head = (uint32)m_segBucket->getNthDatum(hashLoc);
//         m_next->putArray((Datum*)&old_head, NULL, 1);
//         m_segBucket->setNthDatum(hashLoc, (ScalarValue*)&m_rows);
//         printf("  [After] segBucket[%u]: old_head=%u -> new_head=%ld\n", hashLoc, old_head, m_rows);
//     }

//     /* 记录位置 */
//     m_loc[idx] = m_rows;

//     /* 打印哈希值数组中对应项 */
//     if (m_hashVal != NULL) {
//         uint32 stored_hash = ((uint32*)m_hashVal)[m_rows - 1];
//         printf("  [m_hashVal][%ld] = %u\n", m_rows - 1, stored_hash);
//     }

//     printf("  ✅ Insert complete for row %ld\n", m_rows);
//     fflush(stdout);

//     return extra_size_needed;
// }
// int64 SonicHashAgg::insertHashTbl(VectorBatch* batch, int idx, uint32 hashval, uint32 hashLoc)
// {
//     int i;
//     int64 extra_size_needed = 0;
//     ScalarVector* scalar_vec = NULL;

//     m_rows++;

//     /* ==================== 每 1000 行输出一次日志 ==================== */
//     // if (m_rows % 5000 == 0) {
//     //     printf(
//     //         "[SonicHashAgg][Thread %lu][smp_id=%ld] "
//     //         "Processed %ld rows so far (hashval=%u, hashLoc=%u)\n",
//     //         gs_thread_self(),
//     //         u_sess->stream_cxt.smp_id,
//     //         m_rows,
//     //         hashval,
//     //         hashLoc
//     //     );
//     // }
//     // if (m_rows % 5000 == 0) {
//     elog(LOG,
//          "[SonicHashAgg][Thread %lu][smp_id=%ld] "
//          "Processed %ld rows so far (hashval=%u, hashLoc=%u)",
//          gs_thread_self(),
//          u_sess->stream_cxt.smp_id,
//          m_rows,
//          hashval,
//          hashLoc);
//     // }
//     /* =============================================================== */

//     /* ---------- 1. 输入桶数据 ---------- */
//     for (i = 0; i < m_hashNeed; i++) {
//         scalar_vec = &batch->m_arr[m_hashInBatchIdx[i]];
//         m_data[i]->putArray(&scalar_vec->m_vals[idx], &scalar_vec->m_flag[idx], 1);

//         if (likely(NOT_NULL(scalar_vec->m_flag[idx]))) {
//             if (m_tupleCount >= 0 && scalar_vec->m_desc.encoded) {
//                 extra_size_needed += VARSIZE_ANY(scalar_vec->m_vals[idx]);
//             }
//         }
//     }
//     m_colWidth += extra_size_needed;

//     /* ---------- 2. 初始化聚合值 ---------- */
//     for (i = 0; i < m_aggNum; i++) {
//         uint64 init_val = 0;
//         uint8 init_flag = V_NULL_MASK;
//         m_data[m_aggIdx[i]]->putArray(&init_val, &init_flag, 1);

//         if (m_runtime->aggInfo[i].vec_final_function.flinfo != NULL) {
//             m_data[m_aggIdx[i] + 1]->putArray(&init_val, &init_flag, 1);
//         }
//     }

//     /* ---------- 3. 存储哈希值 ---------- */
//     Datum tmp_hashval = UInt32GetDatum(hashval);
//     m_hash->putArray((ScalarValue*)&tmp_hashval, NULL, 1);

//     /* ---------- 4. 更新哈希表链表头 ---------- */
//     if (likely(!m_useSegHashTbl)) {
//         m_next->putArray((Datum*)&(((uint32*)m_bucket)[hashLoc]), NULL, 1);
//         ((uint32*)m_bucket)[hashLoc] = m_rows;
//     } else {
//         uint32 bucket_pos = (uint32)m_segBucket->getNthDatum(hashLoc);
//         m_next->putArray((Datum*)&(bucket_pos), NULL, 1);
//         m_segBucket->setNthDatum(hashLoc, (ScalarValue*)&m_rows);
//     }

//     /* ---------- 5. 记录当前行位置 ---------- */
//     m_loc[idx] = m_rows;

//     return extra_size_needed;
// }

class HashTableSizeStats {
public:
    struct SizeStats {
        size_t total_hash_table_size = 0;    // 整个哈希表估算的总字节数
        size_t key_data_size = 0;           // 键（group key）占用的字节数
        size_t agg_data_size = 0;           // 聚合列占用的字节数
        size_t hash_metadata_size = 0;      // 哈希相关元数据（hash值、next 指针、位置映射）字节数
        size_t bucket_array_size = 0;       // 桶数组（bucket array）占用字节数（固定，只计算一次）
        size_t estimated_cache_lines = 0;   // 按 64B cache line 估算的 cache line 数量
        uint64_t total_inserts = 0;         // 累计插入的行数
        struct timespec last_report_time;   // 上次报告时间戳，用于计算时间差或速率
        uint64_t total_aggregations = 0;  // 当前线程累计聚合次数
    };
    
    static __thread SizeStats tls_size_stats; // 线程局部的统计结构，每个线程/SM P 有自己的统计

    // 每次插入时调用，用参数更新线程局部的统计
    static void recordInsertSize(int hash_need, int agg_num, int64 extra_size, 
                                size_t bucket_size, bool use_seg_hash_tbl, int smp_id,int agg_calls = 0) {
        // 如果是第一次插入，记录起始时间
        if (tls_size_stats.total_inserts == 0) {
            clock_gettime(CLOCK_MONOTONIC, &tls_size_stats.last_report_time);
        }
        
        // 估算本次插入对行大小的增加（以字节为单位）
        size_t row_size = 0;
        
        /* ---------- 1. 键数据大小 ---------- */
        // 假设每个键使用 ScalarValue（通常 8 字节） + flag（1 字节）//typedef uintptr_t ScalarValue;
        tls_size_stats.key_data_size += hash_need * (sizeof(ScalarValue) + sizeof(uint8));
        row_size += hash_need * (sizeof(ScalarValue) + sizeof(uint8));
        
        // 如果有变长数据（extra_size），也加到键数据大小
        tls_size_stats.key_data_size += extra_size;
        row_size += extra_size;
        
        /* ---------- 2. 聚合数据大小 ---------- */
        // 假设每个聚合列占用一个 uint64（8B）和一个 flag（1B）作为 slot
        size_t agg_size_per_col = sizeof(uint64) + sizeof(uint8);
        // 把每个聚合列的空间加到 agg_data_size（中间状态）
        tls_size_stats.agg_data_size += agg_num * agg_size_per_col;
        row_size += agg_num * agg_size_per_col;
        
        // 如果需要 final 状态（例如某些聚合需要额外 slot），再加一次保守估计
        tls_size_stats.agg_data_size += agg_num * agg_size_per_col;
        row_size += agg_num * agg_size_per_col;
        
        /* ---------- 3. 哈希元数据大小 ---------- */
        // 存储哈希值（uint32）
        tls_size_stats.hash_metadata_size += sizeof(uint32);
        row_size += sizeof(uint32);
        
        // 存储链表的 next 指针（uint32，用于链表冲突处理）
        tls_size_stats.hash_metadata_size += sizeof(uint32);
        row_size += sizeof(uint32);
        
        // 存储位置映射（m_loc 中的行号，uint32）
        tls_size_stats.hash_metadata_size += sizeof(uint32);
        row_size += sizeof(uint32);
        
        /* ---------- 4. 哈希桶数组大小 ---------- */
        // 桶数组通常是固定大小，所以只在第一次插入时统计一次
        if (tls_size_stats.bucket_array_size == 0) {
            if (!use_seg_hash_tbl) {
                // 普通哈希：bucket_size（桶数量） * sizeof(uint32)（每个桶一个 32 位索引）
                tls_size_stats.bucket_array_size = bucket_size * sizeof(uint32);
            } else {
                // 分段哈希表（segmented hash table）也按保守估计计算
                tls_size_stats.bucket_array_size = bucket_size * sizeof(uint32); // 保守估计
            }
        }
        
        // 更新总大小（各部分相加）
        tls_size_stats.total_hash_table_size = 
            tls_size_stats.key_data_size + 
            tls_size_stats.agg_data_size + 
            tls_size_stats.hash_metadata_size + 
            tls_size_stats.bucket_array_size;
        
        // 按 64 字节 cache line 估算 cache 行数
        tls_size_stats.estimated_cache_lines = 
            (tls_size_stats.total_hash_table_size + 63) / 64;
        
        // 增加插入计数
        tls_size_stats.total_inserts++;

        tls_size_stats.total_aggregations += agg_calls;
        
        // 每隔 1000 插入打印一次统计报告
        if (tls_size_stats.total_inserts % 1000 == 0) {
            reportSizeStats(smp_id, row_size);
        }
    }
    
    // 打印并报告当前统计（调用 elog 打印日志）
    static void reportSizeStats(int smp_id, size_t avg_row_size) {
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        
        // 计算距离上次报告经过的秒数（尽管当前未直接使用 elapsed_sec）
        double elapsed_sec = (current_time.tv_sec - tls_size_stats.last_report_time.tv_sec) +
                           (current_time.tv_nsec - tls_size_stats.last_report_time.tv_nsec) / 1e9;
        
        // 使用 elog 打印格式化后的统计信息（以 MB 为单位输出大小）
        elog(LOG, 
            "[HashTableSize][SMP%d] "
            "TotalSize: %.2fMB, "
            "Keys: %.2fMB, Aggs: %.2fMB, Meta: %.2fMB, Buckets: %.2fMB, "
            "CacheLines: %zu, AvgRow: %.1fB, Inserts: %lu, AggCalls: %lu",
            smp_id,
            (double)tls_size_stats.total_hash_table_size / (1024 * 1024),
            (double)tls_size_stats.key_data_size / (1024 * 1024),
            (double)tls_size_stats.agg_data_size / (1024 * 1024),
            (double)tls_size_stats.hash_metadata_size / (1024 * 1024),
            (double)tls_size_stats.bucket_array_size / (1024 * 1024),
            tls_size_stats.estimated_cache_lines,
            (double)avg_row_size,
            tls_size_stats.total_inserts,
            tls_size_stats.total_aggregations   // ✅ 新增输出
        );
        
        // 更新上次报告时间戳为当前时间
        tls_size_stats.last_report_time = current_time;
    }
    
    // 返回当前线程局部统计的副本，供外部（CacheHitPredictor）分析使用
    static SizeStats getCurrentStats(int smp_id) {
        return tls_size_stats;
    }
};

// 定义静态成员
thread_local HashTableSizeStats::SizeStats HashTableSizeStats::tls_size_stats;
class CacheHitPredictor {
public:
    // 基于 SizeStats 提供缓存层级的简单命中率预测
    static void analyzeCacheBehavior(const HashTableSizeStats::SizeStats& stats, int smp_id) {
        // 使用常见的缓存容量估算（简化模型）
        /* ---------- L1 Cache 分析 (通常 32-64KB) ---------- */
        size_t l1_cache_size = 32 * 1024; // 32KB
        bool fits_in_l1 = stats.total_hash_table_size <= l1_cache_size;
        
        /* ---------- L2 Cache 分析 (通常 256-512KB per core) ---------- */
        size_t l2_cache_size = 256 * 1024; // 256KB
        bool fits_in_l2 = stats.total_hash_table_size <= l2_cache_size;
        
        /* ---------- L3 Cache 分析 (通常 2-4MB per core) ---------- */
        size_t l3_cache_size = 2 * 1024 * 1024; // 2MB
        bool fits_in_l3 = stats.total_hash_table_size <= l3_cache_size;
        
        /* ---------- 缓存命中率预测 ---------- */
        double predicted_hit_rate = 0.0;
        const char* cache_level = "";
        
        // 根据整表是否能放入某一级缓存，设定一个经验命中率基线
        if (fits_in_l1) {
            predicted_hit_rate = 0.95; // 95% 命中率
            cache_level = "L1";
        } else if (fits_in_l2) {
            predicted_hit_rate = 0.85; // 85% 命中率
            cache_level = "L2";
        } else if (fits_in_l3) {
            predicted_hit_rate = 0.70; // 70% 命中率
            cache_level = "L3";
        } else {
            predicted_hit_rate = 0.30; // 30% 命中率，主要回落到主内存访问
            cache_level = "MainMemory";
        }
        
        // 考虑 bucket 数组（桶）的缓存友好性对命中率的缩放
        double bucket_cache_efficiency = calculateBucketCacheEfficiency(stats.bucket_array_size);
        predicted_hit_rate *= bucket_cache_efficiency; // 最终命中率 = 基线 * 桶效率
        
        // 打印预测结果到日志
        elog(LOG,
            "[CachePredict][SMP%d] "
            "TableSize: %.2fMB -> Fits in %s, "
            "PredictedHitRate: %.1f%%, BucketEfficiency: %.1f%%, "
            "TotalCacheLines: %zu",
            smp_id,
            (double)stats.total_hash_table_size / (1024 * 1024),
            cache_level,
            predicted_hit_rate * 100,
            bucket_cache_efficiency * 100,
            stats.estimated_cache_lines);
    }
    
private:
    // 根据桶数组总体大小给出一个经验性的缓存效率因子
    static double calculateBucketCacheEfficiency(size_t bucket_size) {
        // 小的桶数组更容易被缓存，返回较高效率因子
        if (bucket_size <= 16 * 1024) { // <= 16KB
            return 1.0; // 完全缓存友好
        } else if (bucket_size <= 64 * 1024) { // <= 64KB
            return 0.9; // 大部分缓存友好
        } else if (bucket_size <= 256 * 1024) { // <= 256KB
            return 0.7; // 部分缓存友好
        } else {
            return 0.4; // 较大时缓存不友好
        }
    }
};


int64 SonicHashAgg::insertHashTbl(VectorBatch* batch, int idx, uint32 hashval, uint32 hashLoc)
{
    int i;                                 // 循环变量
    int64 extra_size_needed = 0;           // 本次插入额外的变长字节数
    ScalarVector* scalar_vec = NULL;       // 临时指向输入列的向量

    m_rows++;                              // 全局/本实例的行计数器自增，作为新行的 id

    /* ---------- 1. 输入桶数据 ---------- */
    for (i = 0; i < m_hashNeed; i++) {
        // 在批次中找到第 i 个 key 所在的列（通过映射 m_hashInBatchIdx）
        scalar_vec = &batch->m_arr[m_hashInBatchIdx[i]];
        // 把该列当前行的数据和值的 flag 写入内部 m_data 的对应列
        m_data[i]->putArray(&scalar_vec->m_vals[idx], &scalar_vec->m_flag[idx], 1);

        // 如果该值非 NULL
        if (likely(NOT_NULL(scalar_vec->m_flag[idx]))) {
            // 若启用了 tupleCount 检查并且该列是编码（变长）类型，则计算变长字节并累加
            if (m_tupleCount >= 0 && scalar_vec->m_desc.encoded) {
                extra_size_needed += VARSIZE_ANY(scalar_vec->m_vals[idx]);
            }
        }
    }
    // 把本次插入的变长额外宽度累加到 m_colWidth（用于列宽统计或内存管理）
    m_colWidth += extra_size_needed;

    /* ---------- 2. 初始化聚合值 ---------- */
    for (i = 0; i < m_aggNum; i++) {
        uint64 init_val = 0;                // 聚合初始值（示例使用 0）
        uint8 init_flag = V_NULL_MASK;      // 初始 flag（这里使用 V_NULL_MASK，表示初始/无效状态）
        // 把初始值写到聚合列对应位置
        m_data[m_aggIdx[i]]->putArray(&init_val, &init_flag, 1);

        // 如果该聚合有 final 函数（需要额外的 slot 来保存中间/最终状态），写入额外 slot
        if (m_runtime->aggInfo[i].vec_final_function.flinfo != NULL) {
            m_data[m_aggIdx[i] + 1]->putArray(&init_val, &init_flag, 1);
        }
    }

    /* ---------- 3. 存储哈希值 ---------- */
    Datum tmp_hashval = UInt32GetDatum(hashval); // 将 uint32 hash 转为 Datum 表示
    m_hash->putArray((ScalarValue*)&tmp_hashval, NULL, 1); // 写入哈希列（无 flag）

    /* ---------- 4. 更新哈希表链表头 ---------- */
    if (likely(!m_useSegHashTbl)) {
        // 普通哈希表实现：把旧的桶头写为新节点的 next，然后把桶头更新为当前行（m_rows）
        m_next->putArray((Datum*)&(((uint32*)m_bucket)[hashLoc]), NULL, 1);
        ((uint32*)m_bucket)[hashLoc] = m_rows; // 将桶头指向新行
    } else {
        // 分段哈希表实现：从分段 bucket 读取旧头，作为 next，然后设置新头
        uint32 bucket_pos = (uint32)m_segBucket->getNthDatum(hashLoc);
        m_next->putArray((Datum*)&(bucket_pos), NULL, 1);
        m_segBucket->setNthDatum(hashLoc, (ScalarValue*)&m_rows);
    }

    /* ---------- 5. 记录当前行位置 ---------- */
    m_loc[idx] = m_rows; // 记录该批内行 idx 在哈希表中的行号（用于回写或定位）

    /* ---------- 6. 统计哈希表大小和缓存分析 ---------- */
    // HashTableSizeStats::recordInsertSize(
    //     m_hashNeed, 
    //     m_aggNum, 
    //     extra_size_needed,
    //     m_hashSize,  // bucket 数量
    //     m_useSegHashTbl,
    //     u_sess->stream_cxt.smp_id,
    //     m_aggNum // ✅ 每插入一行，对应 m_aggNum 次聚合初始化
    // );
    
    // // 每 5000 行调用一次缓存行为分析（相比每 1000 的 size 报告，这里周期更长）
    // if (m_rows % 10000 == 0) {
    //     auto current_stats = HashTableSizeStats::getCurrentStats(u_sess->stream_cxt.smp_id);
    //     CacheHitPredictor::analyzeCacheBehavior(current_stats, u_sess->stream_cxt.smp_id);
    // }

    return extra_size_needed; // 返回此次插入导致的变长额外大小
}

/*
 * @Description	: Calculate the used hash size of current sonic hash memory context.
 * @in ctx		: context name.
 * @in memorySize	: pointer to save result size.
 * @in freeSize	: free space in current cxt.
 * @return		: void.
 */
void SonicHashAgg::calcHashContextSize(MemoryContext ctx, int64* memorySize, int64* freeSize)
{
    AllocSetContext* aset = (AllocSetContext*)ctx;
    MemoryContext child;

    if (NULL == ctx) {
        return;
    }

    /* calculate MemoryContext Stats */
    *memorySize += (aset->totalSpace);
    *freeSize += (aset->freeSpace);

    /* recursive MemoryContext's child */
    for (child = ctx->firstchild; child != NULL; child = child->nextchild) {
        calcHashContextSize(child, memorySize, freeSize);
    }
}

/*
 * @Description	: Jude if memory is overflow or not after inserting a new hash value.
 * @in opname	: operator name.
 * @in planId		: Plan node id of current operator.
 * @in dop		: query dop of current session.
 */
void SonicHashAgg::judgeMemoryOverflow(
    char* opname, int planId, int dop, Instrumentation* instrument, int64 size_needed)
{
    int64 used_size = 0;
    int64 free_size = 0;
    bool need_spill = false;
    calcHashContextSize(m_memControl.hashContext, &used_size, &free_size);
    bool sys_busy = gs_sysmemory_busy(used_size * dop, false);
    bool rackBusy = RackMemoryBusy(used_size * dop);
    int64 rackAvail = GetAvailRackMemory(dop) * 1024L;
    int64 localTotalMemory = SET_NODEMEM(u_sess->attr.attr_memory.work_mem, dop) * 1024L;
    u_sess->local_memory_exhaust = used_size > localTotalMemory;

    /*
     * Since if we already have one atom, we could put at least INIT_DATUM_ARRAY_SIZE
     * element without consider varbuf. Once we consume one atom, we need to alloc a
     * new atom.
     */
    if (m_rows % (INIT_DATUM_ARRAY_SIZE - 1) != 0) {
        need_spill = (uint64)used_size > m_memControl.totalMem;
        need_spill = (unsigned int)(need_spill) && (unsigned int)(free_size < size_needed);
    } else {
        need_spill = (uint64)(used_size + m_arrayExpandSize) > m_memControl.totalMem;
    }

    /* Record spill info or try to spread memory */
    if (need_spill || sys_busy || (u_sess->local_memory_exhaust && rackBusy)) {
        if (m_memControl.spillToDisk == false) {
            AllocSetContext* set = (AllocSetContext*)(m_memControl.hashContext);
            if (sys_busy || (u_sess->local_memory_exhaust && rackBusy)) {
                MEMCTL_LOG(LOG,
                    "%s(%d) early spilled, workmem: %luKB, usedmem: %ldKB, "
                    "sonic hash context freeSpace: %ldKB.",
                    opname,
                    planId,
                    m_memControl.totalMem / 1024L,
                    used_size / 1024L,
                    free_size / 1024L);
                m_memControl.sysBusy = true;
                m_memControl.totalMem = used_size;
                set->maxSpaceSize = used_size;
                pgstat_add_warning_early_spill();
            } else if (m_memControl.maxMem > m_memControl.totalMem) {
                /* try to spread mem, and record width if failed */
                m_memControl.totalMem = used_size;
                int64 spreadMem = Min(Min((uint64)dywlm_client_get_memory() * 1024L, m_memControl.totalMem),
                    m_memControl.maxMem - m_memControl.totalMem);
                if (spreadMem > m_memControl.totalMem * MEM_AUTO_SPREAD_MIN_RATIO) {
                    m_memControl.totalMem += spreadMem;
                    m_memControl.spreadNum++;
                    set->maxSpaceSize += spreadMem;

                    MEMCTL_LOG(DEBUG2,
                        "[%s(%d)]: auto mem spread %ldKB succeed, and work mem is %luKB.",
                        opname,
                        planId,
                        spreadMem / 1024L,
                        m_memControl.totalMem / 1024L);
                    return;
                }

                MEMCTL_LOG(LOG,
                    "[%s(%d)]: auto mem spread %ldKB failed, and work mem is %luKB.",
                    opname,
                    planId,
                    spreadMem / 1024L,
                    m_memControl.totalMem / 1024L);
                if (m_memControl.spreadNum > 0) {
                    pgstat_add_warning_spill_on_memory_spread();
                }
            }

            if (m_tupleCount != 0) {
                m_colWidth /= m_tupleCount;
            }
            m_tupleCount = -1;
        }

        /*
         * used_size is the available size can be used in calculating hashsize for each temp file.
         */
        m_memControl.availMem = used_size;

        /* next slot will be inserted into temp file */
        if (m_memControl.spillToDisk == true) {
            m_strategy = HASH_RESPILL;
        } else {
            ereport(
                LOG, (errmodule(MOD_VEC_EXECUTOR), errmsg("Profiling Warning : %s(%d) Disk Spilled.", opname, planId)));

            /* first time spill to disk */
            m_fill_table_rows = m_rows;
            m_strategy = HASH_IN_DISK;

            /* cache the memory size into instrument for explain performance */
            if (instrument != NULL) {
                instrument->memoryinfo.peakOpMemory = used_size;
            }
        }
    }
}

/*
 * @Description	: Judge current memory status is allowed for expand hash table or not.
 * @return		: Return true if expand hash table is allowed by memory.
 */
bool SonicHashAgg::judgeMemoryAllowExpand()
{
    int64 used_size = 0;
    int64 free_size = 0;

    calcHashContextSize(m_memControl.hashContext, &used_size, &free_size);

    if (m_memControl.totalMem >= (uint64)(used_size * HASH_EXPAND_SIZE)) {
        ereport(DEBUG2,
            (errmodule(MOD_VEC_EXECUTOR),
                errmsg("Allow Sonic Hash Expand: "
                       "avialmem: %luKB, current HashContext, totalSpace: %ldKB, freeSpace: %ldKB",
                    m_memControl.totalMem / 1024L,
                    used_size / 1024L,
                    free_size / 1024L)));
        return true;
    } else {
        return false;
    }
}

/*
 * @Description	: Compute sizing parameters for hashtable.
 * @in oldSize		: old hash table size
 * @return		: new size for building hash table
 */
template <bool expand, bool logit>
int64 SonicHashAgg::calcHashTableSize(int64 oldSize)
{
    int64 hash_size;
    int64 sizepow;
    int64 allowed_size;

    if (expand == false) {
#ifdef USE_PRIME
        hash_size = (uint32)hashfindprime(oldSize);
#else
        /* supporting zero sized hashes would complicate matters */
        hash_size = Max(oldSize, MIN_HASH_TABLE_SIZE);

        /* round up size to the next power of 2, that's the bucketing works */
        hash_size = 1L << my_log2(hash_size);
#endif

        allowed_size = m_memControl.totalMem / sizeof(uint32);

        sizepow = 1UL << (unsigned int)my_log2(allowed_size);

        if (allowed_size != sizepow) {
            allowed_size = sizepow / 2;
        }

        if (hash_size * HASH_EXPAND_SIZE > allowed_size) {
            m_enableExpansion = false;
        } else {
            m_enableExpansion = true;
        }

        if (logit) {
            MEMCTL_LOG(DEBUG2,
                "[VecSonicHashAgg(%d)]: max table size allowed by memory is %ld",
                m_runtime->ss.ps.plan->plan_node_id,
                allowed_size);
        }
    } else {
        allowed_size = m_memControl.totalMem / sizeof(uint32);
        Assert(oldSize * HASH_EXPAND_SIZE <= allowed_size);

        hash_size = oldSize * HASH_EXPAND_SIZE;
#ifdef USE_PRIME
        hash_size = (uint32)hashfindprime(hash_size);
#else
        /* supporting zero sized hashes would complicate matters */
        hash_size = Max(hash_size, MIN_HASH_TABLE_SIZE);

        /* round up size to the next power of 2, that's the bucketing works  */
        hash_size = 1L << my_log2(hash_size);
#endif

        if (hash_size * HASH_EXPAND_SIZE > allowed_size) {
            m_enableExpansion = false;
        }
        else {
            m_enableExpansion = true;
        }
    }
    // if (logit) {
        // elog(LOG,
        //     "[VecSonicHashAgg(%d)]: Hash table old size is %ld, new size is %ld, m_rows :%ld.",
        //     m_runtime->ss.ps.plan->plan_node_id,
        //     oldSize,
        //     hash_size,
        //     m_rows);
    // }
    // if (logit) {
    //     MEMCTL_LOG(DEBUG2,
    //         "[VecSonicHashAgg(%d)]: Hash table old size is %ld, new size is %ld, m_rows :%ld.",
    //         m_runtime->ss.ps.plan->plan_node_id,
    //         oldSize,
    //         hash_size,
    //         m_rows);
    // }

    return hash_size;
}

/*
 * @Description	: collect numbers of rows exceeds rows_in_mem for all partitions
 * 				   with index >= m_currPartIdx
 * @in rows_in_mem	: number of rows already in memory.
 */
int64 SonicHashAgg::calcLeftRows(int64 rows_in_mem)
{
    int64 left_rows = 0;
    int64 add_rows = 0;

    for (int i = m_currPartIdx; i < m_partNum; i++) {
        if (m_partFileSource[i]->m_rows > rows_in_mem) {
            add_rows = m_partFileSource[i]->m_rows - rows_in_mem;
        } else {
            add_rows = 0;
        }

        left_rows += add_rows;
    }

    return left_rows;
}

/*
 * @Description	: Insert idx-th batch element into hash table and judge memory status.
 * @in batch		: The batch that we need to deal with.
 * @in idx		: The position of current element in batch we need to handle.
 * @in hashLoc	: The position of this batch element's hashval in m_bucket.
 */
void SonicHashAgg::AllocHashTbl(VectorBatch* batch, int idx, uint32 hashval, int hashLoc)
{
    ScalarVector* pVector = NULL;
    uint16 part_idx;

    if (m_tupleCount >= 0) {
        m_tupleCount++;
    }
    switch (m_strategy) {
        case HASH_IN_MEMORY: {
            AutoContextSwitch memSwitch(m_memControl.hashContext);

            /* first add the fixed sonic array element size which is defined in initialization */
            if (m_tupleCount >= 0) {
                m_colWidth += m_arrayElementSize;
            }

            int64 size_needed = insertHashTbl(batch, idx, hashval, hashLoc);
            /* judge memory status after inserting */
            judgeMemoryOverflow("VecSonicHashAgg",
                m_runtime->ss.ps.plan->plan_node_id,
                SET_DOP(m_runtime->ss.ps.plan->dop),
                m_runtime->ss.ps.instrument,
                size_needed);
        } break;

        case HASH_IN_DISK: {
            /* spill to disk first time */
            m_loc[idx] = 0;
            WaitState oldState = pgstat_report_waitstatus(STATE_EXEC_HASHAGG_WRITE_FILE);
            if (unlikely(false == m_memControl.spillToDisk)) {
                m_partNum = calcPartitionNum(((VecAgg*)m_runtime->ss.ps.plan)->numGroups);
                ereport(LOG,
                    (errmodule(MOD_VEC_EXECUTOR),
                        errmsg("[VecSonicHashAgg(%d)]: "
                               "first time spill partition num: %d.",
                            m_runtime->ss.ps.plan->plan_node_id,
                            m_partNum)));

                if (m_partFileSource == NULL) {
                    m_partFileSource = createPartition(m_partNum);
                    /* make sure current partition index */
                    m_currPartIdx = -1;
                } else {
                    resetVariableMemberIfNecessary(m_partNum);
                    Assert(0);
                }

                if (m_runtime->ss.ps.instrument) {
                    m_runtime->ss.ps.instrument->sorthashinfo.hash_FileNum = m_partNum * m_buildOp.cols;
                    m_runtime->ss.ps.instrument->sorthashinfo.hash_writefile = true;
                    m_runtime->ss.ps.instrument->sorthashinfo.hash_spillNum = 0;
                }

                pgstat_increase_session_spill();

                m_memControl.spillToDisk = true;

                /* Do not expand hashtable if convert to wirte table */
                m_enableExpansion = false;
            }

            /*
             * Compute the hash value and save to disk, need to wrap this function, first
             * mark which partition this element belongs to
             */
            HashKey key = DatumGetUInt32(hash_uint32(hashval));
#ifdef USE_PRIME
            part_idx = key % m_partNum;
#else
            part_idx = key & (m_partNum - 1);
#endif
            if (u_sess->attr.attr_sql.enable_sonic_optspill) {
                for (int k = 0; k < m_partFileSource[part_idx]->m_cols; k++) {
                    pVector = &batch->m_arr[k];
                    SonicHashFilePartition* partFileSource = (SonicHashFilePartition*)m_partFileSource[part_idx];
                    partFileSource->putVal<true>(&pVector->m_vals[idx], &pVector->m_flag[idx], k);
                }
            } else {
                for (int k = 0; k < m_partFileSource[part_idx]->m_cols; k++) {
                    pVector = &batch->m_arr[k];
                    SonicHashFilePartition* partFileSource = (SonicHashFilePartition*)m_partFileSource[part_idx];
                    partFileSource->putVal<false>(&pVector->m_vals[idx], &pVector->m_flag[idx], k);
                }
            }
            m_partFileSource[part_idx]->m_rows += 1;
            (void)pgstat_report_waitstatus(oldState);

            // /* ✅ 统计插入行信息（用于统一线程统计） */
            // HashTableSizeStats::recordInsertSize(
            //     m_hashNeed,
            //     m_aggNum,
            //     0,                      // 溢写不占额外内存
            //     m_hashSize,             // 桶数可复用
            //     m_useSegHashTbl,
            //     u_sess->stream_cxt.smp_id,
            //     m_aggNum                // 聚合列次数统计
            // );

        } break;

        case HASH_RESPILL: {
            /* respill to disk */
            m_loc[idx] = 0;
            WaitState oldStatus = pgstat_report_waitstatus(STATE_EXEC_HASHAGG_WRITE_FILE);
            if (m_overflowFileSource == NULL) {
                /* calculate */
                int64 rows = calcLeftRows(m_fill_table_rows);
                int partNum = getPower2NextNum(rows / m_fill_table_rows);
                partNum = Max(2, partNum);
                partNum = Min(partNum, HASH_MAX_FILENUMBER);

                ereport(LOG,
                    (errmodule(MOD_VEC_EXECUTOR),
                        errmsg("[VecSonicHashAgg(%d)]: current "
                               "respill file idx: %d, its file rows: %ld, m_fill_table_rows: %d, all redundant "
                               "rows: %ld, respill partition num: %d.",
                            m_runtime->ss.ps.plan->plan_node_id,
                            m_currPartIdx,
                            m_partFileSource[m_currPartIdx]->m_rows,
                            m_fill_table_rows,
                            rows,
                            partNum)));

                /* create new partition */
                m_overflowNum = partNum;
                m_overflowFileSource = createPartition(m_overflowNum);

                /* record this status and warning */
                m_memControl.spillNum++;
                if (m_memControl.spillNum >= WARNING_SPILL_TIME) {
                    t_thrd.shemem_ptr_cxt.mySessionMemoryEntry->warning =
                        ((unsigned int)t_thrd.shemem_ptr_cxt.mySessionMemoryEntry->warning) |
                        (1 << WLM_WARN_SPILL_TIMES_LARGE);
                }

                if (m_runtime->ss.ps.instrument) {
                    m_runtime->ss.ps.instrument->sorthashinfo.hash_spillNum++;
                    m_runtime->ss.ps.instrument->sorthashinfo.hash_FileNum += m_partNum * m_buildOp.cols;

                    if (m_memControl.spillNum >= WARNING_SPILL_TIME) {
                        m_runtime->ss.ps.instrument->warning |= (1 << WLM_WARN_SPILL_TIMES_LARGE);
                    }
                }
            }

            /* Compute the hash value for tuple and resave to disk, first mark
             * which partition this element belongs to */
            HashKey key = DatumGetUInt32(hash_uint32(hashval));
#ifdef USE_PRIME
            part_idx = key % m_overflowNum;
#else
            part_idx = key & (m_overflowNum - 1);
#endif
            if (u_sess->attr.attr_sql.enable_sonic_optspill) {
                for (int j = 0; j < m_overflowFileSource[part_idx]->m_cols; j++) {
                    pVector = &batch->m_arr[j];
                    SonicHashFilePartition* overflowFileSource = 
                        (SonicHashFilePartition*)m_overflowFileSource[part_idx];
                    overflowFileSource->putVal<true>(&pVector->m_vals[idx], &pVector->m_flag[idx], j);
                }
            } else {
                for (int j = 0; j < m_overflowFileSource[part_idx]->m_cols; j++) {
                    pVector = &batch->m_arr[j];
                    SonicHashFilePartition* overflowFileSource = 
                        (SonicHashFilePartition*)m_overflowFileSource[part_idx];
                    overflowFileSource->putVal<false>(&pVector->m_vals[idx], &pVector->m_flag[idx], j);
                }
            }
            m_overflowFileSource[part_idx]->m_rows += 1;
            (void)pgstat_report_waitstatus(oldStatus);

            // /* ✅ 同样进行一次统计更新 */
            // HashTableSizeStats::recordInsertSize(
            //     m_hashNeed,
            //     m_aggNum,
            //     0,
            //     m_hashSize,
            //     m_useSegHashTbl,
            //     u_sess->stream_cxt.smp_id,
            //     m_aggNum
            // );

        } break;
        default:
            ereport(ERROR,
                (errcode(ERRCODE_UNDEFINED_OBJECT),
                    errmodule(MOD_VEC_EXECUTOR),
                    (errmsg("Unrecognized vector sonic hash aggregation status."))));
            break;
    }
}

/*
 * @Description	: Check if expand hash table is needed.
 */
void SonicHashAgg::tryExpandHashTable()
{
    /*
     * Expand hash table if needed for each batch.
     */
    if (m_enableExpansion && m_rows >= m_hashSize * HASH_EXPAND_THRESHOLD) {
        /*
         * Judge memory is enough for expanding hashtable.
         */
        if ((m_memControl.maxMem > 0) || judgeMemoryAllowExpand()) {
            m_hashSize = calcHashTableSize<true, true>(m_hashSize);
            expandHashTable();
            // m_hashSize = calcHashTableSize<true, true>(m_hashSize);

        // elog(LOG,
        //     "[VecSonicHashAgg(%d) smp(%d)]: Hash table old size is %ld, new size is %ld, m_rows :%ld.",
        //     m_runtime->ss.ps.plan->plan_node_id,
        //     u_sess->stream_cxt.smp_id,
        //     m_hashSize,
        //     m_rows);
        } else {
            m_enableExpansion = false;
        }
    }
}

/*
 * @Description	: expand hash table with new hash size
 */
void SonicHashAgg::expandHashTable()
{
    instr_time start_time;
    double total_time;
    uint32 hash_val;
    uint32 hash_loc;
    int i;

    int64 rows_num = 0;
    errno_t rc = 0;

    /* fack case : no data left in hash table */
    if (m_rows == 0) {
        return;
    }

    AutoContextSwitch memSwitch(m_memControl.hashContext);

    /* repalloc m_next */
    DatumDesc desc;
    getDataDesc(&desc, 4, NULL, false);

    for (i = 0; i < m_next->m_arrIdx + 1; i++) {
        pfree(m_next->m_arr[i]->data);
        pfree(m_next->m_arr[i]);
    }
    m_next = New(CurrentMemoryContext)
        SonicIntTemplateDatumArray<uint32>(m_memControl.hashContext, m_atomSize, false, &desc);

    /*
     * if use segment hash bucket, reset the old part and append new atom according to the
     * new calculated hash size.
     */
    if (m_useSegHashTbl) {
        int32 oldSegNum = m_segNum;
        /* calculate new segment number according to new hash table size */
        m_segNum = (m_hashSize - 1) / INIT_DATUM_ARRAY_SIZE + 1;

        /* reset the old part */
        for (i = 0; i < oldSegNum; i++) {
            rc = memset_s(m_segBucket->m_arr[i]->data,
                sizeof(uint32) * m_segBucket->m_atomSize,
                0,
                sizeof(uint32) * m_segBucket->m_atomSize);
            securec_check(rc, "", "");
        }

        /* append new atom that needed */
        for (int j = oldSegNum; j < m_segNum; j++)
            m_segBucket->genNewArray(false);
        m_segBucket->m_curAtom = m_segBucket->m_arr[0];
    } else {
        pfree(m_bucket);
        m_bucket = NULL;

        /* if new hash table exceeds MaxAllocSize , turn to segment bucket */
        if ((uint64)(sizeof(uint32) * m_hashSize) >= (uint64)MaxAllocSize) {
            DatumDesc description;
            getDataDesc(&description, 4, NULL, false);
            m_useSegHashTbl = true;

            if (((Agg *) m_runtime->ss.ps.plan)->unique_check) {
                m_buildFun = &SonicHashAgg::buildAggTblBatch<true, true>;
            } else {
                m_buildFun = &SonicHashAgg::buildAggTblBatch<true, false>;
            }

            m_segBucket = New(CurrentMemoryContext)
                SonicIntTemplateDatumArray<uint32>(m_memControl.hashContext, m_atomSize, false, &description);
            m_segNum = (m_hashSize - 1) / INIT_DATUM_ARRAY_SIZE + 1;
            for (int j = 0; j < m_segNum; j++)
                m_segBucket->genNewArray(false);
            m_segBucket->m_curAtom = m_segBucket->m_arr[0];
        } else {
            /* still use array structure */
            m_bucket = (char*)palloc0(sizeof(uint32) * m_hashSize);
        }
    }

    INSTR_TIME_SET_CURRENT(start_time);

    for (uint32 idx = 1; idx <= m_rows; idx++) {
        hash_val = (uint32)m_hash->getNthDatum(idx);

        rows_num++;

#ifdef USE_PRIME
        hash_loc = hash_val % m_hashSize;
#else
        hash_loc = hash_val & (m_hashSize - 1);
#endif

        if (likely(!m_useSegHashTbl)) {
            m_next->putArray((Datum*)&(((uint32*)m_bucket)[hash_loc]), NULL, 1);
            ((uint32*)m_bucket)[hash_loc] = rows_num;
        } else {
            uint32 bucketPos = (uint32)m_segBucket->getNthDatum(hash_loc);
            m_next->putArray((Datum*)&(bucketPos), NULL, 1);
            m_segBucket->setNthDatum(hash_loc, (ScalarValue*)&rows_num);
        }
    }

    total_time = elapsed_time(&start_time);
    Assert(rows_num == m_rows);

    /* record expand times */
    if (m_runtime->ss.ps.instrument) {
        m_runtime->ss.ps.instrument->sorthashinfo.hashtable_expand_times++;
    }
}

/*
 * @Description	: Calculate partition number of current routine.
 * @return		: return partition number.
 */
uint16 SonicHashAgg::calcPartitionNum(long numGroups)
{
    int estsize = getPower2LessNum(2 * numGroups / m_rows);
    int partNum = Max(HASH_MIN_FILENUMBER, estsize);
    partNum = Min(partNum, HASH_MAX_FILENUMBER);

    return partNum;
}

/*
 * @Description	: Reset member information of current partition status.
 * @in partNum	: number of partitions.
 */
void SonicHashAgg::resetVariableMemberIfNecessary(int partNum)
{
    /*
     * Since each partiton has self-context, we should reset them one by one.
     */
    for (int i = 0; i < m_partNum; i++) {
        if (m_partFileSource[i]->m_context != NULL) {
            MemoryContextReset(m_partFileSource[i]->m_context);
        }
    }

    if (partNum <= m_partNum) {
        for (int i = 0; i < m_partNum; i++) {
            m_partFileSource[i]->m_rows = 0;
            m_partFileSource[i]->m_size = 0;
        }
    }
}

/*
 * @Description	: initialize each hash partition
 * @in partSource	: sonic hash partition data structure.
 */
void SonicHashAgg::initPartition(SonicHashPartition** partSource)
{
    MemoryContext oldCxt = MemoryContextSwitchTo((*partSource)->m_context);
    DatumDesc desc;

    TupleDesc outDesc = outerPlanState(m_runtime)->ps_ResultTupleSlot->tts_tupleDescriptor;

    FormData_pg_attribute* attrs = outDesc->attrs;
    for (int idx = 0; idx < m_sourceBatch->m_cols; idx++) {
        getDataDesc(&desc, 0, &attrs[idx], isHashKey(attrs[idx].atttypid, idx, m_buildOp.keyIndx, m_buildOp.keyNum));
        (*partSource)->init(idx, &desc);
    }

    (void)MemoryContextSwitchTo(oldCxt);
}

/*
 * @Description	: create hash partition for hash file source
 * @return		: return hash partition.
 */
SonicHashPartition** SonicHashAgg::createPartition(uint16 num_partitions)
{
    m_sourceBatch = New(CurrentMemoryContext) VectorBatch(CurrentMemoryContext, m_outBatch);

    SonicHashPartition** partFileSource = (SonicHashPartition**)palloc0(sizeof(SonicHashPartition*) * num_partitions);
    for (int i = 0; i < num_partitions; i++) {
        partFileSource[i] = New(CurrentMemoryContext)
            SonicHashFilePartition((char*)"PartitionFileContext", m_sourceBatch, m_memControl.totalMem);

        initPartition(&(partFileSource[i]));
    }

    ereport(DEBUG2,
        (errmodule(MOD_VEC_EXECUTOR),
            errmsg("[VecSonicHashAgg(%d)]: Successed to create %d file patitions.",
                m_runtime->ss.ps.plan->plan_node_id,
                num_partitions)));

    return partFileSource;
}

/*
 * @Description	: Calculate agg function result with respect to one selected column.
 * @in aggInfo	: The Vector Agg structure information.
 * @in pVector	: The input ScalarVector data info.
 * @in idx		: The position of column in m_data structure used to record agg result.
 */
void SonicHashAgg::AggregationOnScalar(VecAggInfo* aggInfo, ScalarVector* pVector, int idx)
{
    AutoContextSwitch memGuard(m_econtext->ecxt_per_tuple_memory);
    FunctionCallInfo fcinfo = &aggInfo->vec_agg_function;

    fcinfo->arg[0] = (Datum)pVector;
    fcinfo->arg[1] = (Datum)idx;
    fcinfo->arg[2] = (Datum)m_loc;
    fcinfo->arg[3] = (Datum)m_data;

    VecFunctionCallInvoke(fcinfo);
    ResetExprContext(m_econtext);
}

/*
 * @Description	: Project and compute aggregation.
 * @in batch		: current batch need to dealed with
 */
// void SonicHashAgg::BatchAggregation(VectorBatch* batch)
// {
//     int i;
//     int nrows;

//     nrows = batch->m_rows;

//     for (i = 0; i < m_aggNum; i++) {
//         VectorBatch* pBatch = NULL;
//         ScalarVector* pVector = NULL;
//         VecAggStatePerAgg peraggstate = &m_runtime->pervecagg[m_aggNum - 1 - i];
//         ExprContext* econtext = NULL;

//         /* for count(*), peraggstate->evalproj is null. */
//         if (peraggstate->evalproj != NULL) {
//             econtext = peraggstate->evalproj->pi_exprContext;
//             econtext->ecxt_outerbatch = batch;
//             pBatch = ExecVecProject(peraggstate->evalproj);
//             Assert(!peraggstate->evalproj || (pBatch->m_cols == 1));
//             pVector = &pBatch->m_arr[0];
//         } else {
//             pVector = &batch->m_arr[0];
//         }

//         pVector->m_rows = Min(pVector->m_rows, nrows);

//         /* do aggregation on one column */
//         AggregationOnScalar(&m_runtime->aggInfo[i], pVector, m_aggIdx[i]);

//         if (econtext != NULL) {
//             ResetExprContext(econtext);
//         }
//     }
// }

/*
 * @Description	: set value to scanBatch include field value and agg value.
 * @in idx		: the localtion of the value we need to set.
 */
void SonicHashAgg::BuildScanBatchSimple(int idx)
{
    int i;
    int nrows = m_scanBatch->m_rows;
    ScalarVector* pVector = NULL;
    for (i = 0; i < m_buildOp.cols; i++) {
        pVector = &m_scanBatch->m_arr[i];
        m_data[i]->getNthDatumFlag(idx, &pVector->m_vals[nrows], &pVector->m_flag[nrows]);
        pVector->m_rows++;
    }
    m_scanBatch->m_rows++;
}

/*
 * @Description	: compute final agg and set value to scanBatch include field value and agg value.
 * @in idx		: the localtion of the value we need to set.
 */
void SonicHashAgg::BuildScanBatchFinal(int idx)
{
    int i, j;
    int nrows = m_scanBatch->m_rows;
    int col_idx = 0;
    ScalarVector* scalar_vector = NULL;

    ExprContext* econtext = m_runtime->ss.ps.ps_ExprContext;
    AutoContextSwitch memGuard(econtext->ecxt_per_tuple_memory);

    j = 0;
    for (i = 0; i < m_buildOp.cols; i++) {
        scalar_vector = &m_scanBatch->m_arr[col_idx];
        if (i == m_finalAggInfo[j].idx) {
            /* get agg and count columns */
            FunctionCallInfo fcinfo = &m_finalAggInfo[j].info->vec_final_function;
            fcinfo->arg[0] = (Datum)m_data;
            fcinfo->arg[1] = (Datum)i;
            fcinfo->arg[2] = (Datum)scalar_vector;
            fcinfo->arg[3] = (Datum)idx;

            FunctionCallInvoke(fcinfo);

            scalar_vector->m_rows++;
            j++; /* next final agg function */
            i++; /* skip the count column */
        } else {
            /* get agg column */
            m_data[i]->getNthDatumFlag(idx, &scalar_vector->m_vals[nrows], &scalar_vector->m_flag[nrows]);
            scalar_vector->m_rows++;
        }

        /* next result column */
        col_idx++;
    }

    m_scanBatch->m_rows++;
}

/*
 * @Description	: Produce the result batch by executing qual and projection, and return back to client.
 * @return		: The finale result of agg node.
 */
VectorBatch* SonicHashAgg::ProducerBatch()
{
    ExprContext* expr_context = NULL;
    VectorBatch* res = NULL;

    /* Guard when there is no input rows */
    if (m_proBatch == NULL) {
        return NULL;
    }

    for (int i = 0; i < m_hashNeed; i++) {
        m_outBatch->m_arr[m_hashInBatchIdx[i]] = m_scanBatch->m_arr[i];
    }
    m_outBatch->m_rows = m_scanBatch->m_rows;

    if (list_length((List*)m_runtime->ss.ps.qual) != 0) {
        ScalarVector* pVector = NULL;

        expr_context = m_runtime->ss.ps.ps_ExprContext;
        expr_context->ecxt_scanbatch = m_scanBatch;
        expr_context->ecxt_aggbatch = m_scanBatch;
        expr_context->ecxt_outerbatch = m_outBatch;

        pVector = ExecVecQual((List*)m_runtime->ss.ps.qual, expr_context, false);

        if (pVector == NULL) {
            return NULL;
        }

        m_scanBatch->Pack(expr_context->ecxt_scanbatch->m_sel);
    }

    for (int i = 0; i < m_hashNeed; i++) {
        m_proBatch->m_arr[m_hashInBatchIdx[i]] = m_scanBatch->m_arr[i];
    }

    /* Do the copy out projection.*/
    m_proBatch->m_rows = m_scanBatch->m_rows;

    expr_context = m_runtime->ss.ps.ps_ExprContext;
    expr_context->ecxt_outerbatch = m_proBatch;
    expr_context->ecxt_aggbatch = m_scanBatch;
    expr_context->m_fUseSelection = m_runtime->ss.ps.ps_ExprContext->m_fUseSelection;
    res = ExecVecProject(m_runtime->ss.ps.ps_ProjInfo);

    return res;
}

// void SonicHashAgg::analyze_aggregation_access(VecAggInfo* aggInfo, ScalarVector* vec, int aggIdx, int nrows)
// {
//     // 确保 group_access_counters 大小足够
//     if (group_access_counters.size() < static_cast<size_t>(nrows)) {
//         group_access_counters.resize(nrows, 0);
//     }

//     std::map<uint32_t, size_t> agg_state_access;  // 聚合状态访问次数统计
//     size_t total_accesses = 0;
//     size_t max_accesses = 0;
//     uint32_t hottest_agg_state = 0;

//     for (int row = 0; row < nrows; row++) {
//         uint32_t agg_loc = m_loc[row];  // 假设 m_loc[row] 是该行对应的聚合状态位置
//         agg_state_access[agg_loc]++;
//         group_access_counters[agg_loc]++;  // 更新全局统计
//         total_accesses++;

//         if (agg_state_access[agg_loc] > max_accesses) {
//             max_accesses = agg_state_access[agg_loc];
//             hottest_agg_state = agg_loc;
//         }
//     }

//     double access_imbalance = (double)max_accesses / (total_accesses / (double)agg_state_access.size());

//     // 输出访问分析信息
//     printf("Aggregation Index %d Access Analysis:\n", aggIdx);
//     printf("   - Total Agg States Accessed: %zu\n", agg_state_access.size());
//     printf("   - Hottest State ID: %u, Accesses: %zu\n", hottest_agg_state, max_accesses);
//     printf("   - Access Imbalance Ratio: %.2fx\n", access_imbalance);

//     if (access_imbalance > 10.0) {
//         printf("   ⚠️ HIGH ACCESS IMBALANCE - Memory contention likely\n");
//     }
// }


// void SonicHashAgg::record_probe_performance(int thread_id, uint64_t duration_ns, int nrows)
// {
//     std::lock_guard<std::mutex> lock(probe_perf_mutex);  // 保证线程安全

//     // 扩容向量以支持更多线程
//     if (thread_id >= static_cast<int>(thread_agg_times.size())) {
//         thread_agg_times.resize(thread_id + 1, 0.0);
//         thread_processed_rows.resize(thread_id + 1, 0);
//     }

//     thread_agg_times[thread_id] += static_cast<double>(duration_ns);  // 累计耗时
//     thread_processed_rows[thread_id] += nrows;                        // 累计处理行数

//     // 可选：每次记录时打印
//     printf("Thread %d: +%d rows, duration %.3f ms, total rows %zu, total time %.3f ms\n",
//            thread_id,
//            nrows,
//            duration_ns / 1e6,
//            thread_processed_rows[thread_id],
//            thread_agg_times[thread_id] / 1e6);
// }

void SonicHashAgg::BatchAggregation(VectorBatch* batch)
{
    int i;
    int nrows;
    instr_time start_time;
    double project_time, agg_time;
    nrows = batch->m_rows;
    for (i = 0; i < m_aggNum; i++) {
        VectorBatch* pBatch = NULL;
        ScalarVector* pVector = NULL;
        VecAggStatePerAgg peraggstate = &m_runtime->pervecagg[m_aggNum - 1 - i];
        ExprContext* econtext = NULL;

        /* for count(*), peraggstate->evalproj is null. */
        if (peraggstate->evalproj != NULL) {
            econtext = peraggstate->evalproj->pi_exprContext;
            econtext->ecxt_outerbatch = batch;
            // INSTR_TIME_SET_CURRENT(start_time);
            pBatch = ExecVecProject(peraggstate->evalproj);
            // INSTR_TIME_SET_CURRENT(end_time);
            // project_time = elapsed_time(&start_time); // 转换为毫秒
            Assert(!peraggstate->evalproj || (pBatch->m_cols == 1));
            pVector = &pBatch->m_arr[0];
            // elog(LOG, "ExecVecProject time: %d ms", project_time);
        } else {
            pVector = &batch->m_arr[0];
        }

        pVector->m_rows = Min(pVector->m_rows, nrows);
        // INSTR_TIME_SET_CURRENT(start_time);
        /* do aggregation on one column */
        AggregationOnScalar(&m_runtime->aggInfo[i], pVector, m_aggIdx[i]);
        // agg_time = elapsed_time(&start_time); // 转换为毫秒
        
        // elog(LOG, "AggregationOnScalar time: %d ms", agg_time);

        if (econtext != NULL) {
            ResetExprContext(econtext);
        }
    }
}

// void SonicHashAgg::analyze_probe_scalability(int num_threads) 
// {
    // printf("=== Batch Aggregation (Probe) Scalability Analysis ===\n");
    
    // // 分析1: 聚合状态访问模式
    // analyze_aggregation_access_pattern(m_proBatch);
    
    // // 分析2: 计算与内存访问比例
    // analyze_compute_memory_ratio();
    
    // // 分析3: 线程间工作负载平衡
    // analyze_workload_balance(num_threads);
    
    // 分析4: 函数调用开销
    //analyze_function_call_overhead();
// }

// void SonicHashAgg::analyze_aggregation_access_pattern(VectorBatch* batch)
// {
//     // printf("1. Aggregation State Access Pattern:\n");
    
//     // // 统计每个聚合状态的访问频率
//     // std::map<uint32_t, size_t> agg_state_access;
//     // size_t total_accesses = 0;
//     // size_t max_accesses = 0;
//     // uint32_t hottest_agg_state = 0;
    
//     // // 模拟或实际统计聚合状态的访问
//     // for (int i = 0; i < m_aggNum; i++) {
//     //     for (int row = 0; row < batch->m_rows; row++) {
//     //         uint32_t agg_loc = m_loc[row];  // 聚合状态位置
//     //         agg_state_access[agg_loc]++;
//     //         total_accesses++;
            
//     //         if (agg_state_access[agg_loc] > max_accesses) {
//     //             max_accesses = agg_state_access[agg_loc];
//     //             hottest_agg_state = agg_loc;
//     //         }
//     //     }
//     // }
    
//     // double access_imbalance = (double)max_accesses / (total_accesses / agg_state_access.size());
//     // printf("   - Total Agg States: %zu\n", agg_state_access.size());
//     // printf("   - Access Imbalance Ratio: %.2fx\n", access_imbalance);
//     // printf("   - Hottest State Accesses: %zu\n", max_accesses);
    
//     // if (access_imbalance > 10.0) {
//     //     printf("   ⚠️ HIGH ACCESS IMBALANCE - Memory contention likely\n");
//     // }
// }

// 假设 AggInfo 定义了聚合类型和相关信息
// 这里是一个简单示例
// double SonicHashAgg::estimate_agg_complexity(VecAggInfo* aggInfo)
// {
//     if (!aggInfo) return 0.0;

//     switch (aggInfo->aggType) {
//         case AGG_SUM:
//             return 1.0; // SUM: 1 次加法
//         case AGG_COUNT:
//             return 1.0; // COUNT: 1 次计数
//         case AGG_AVG:
//             return 2.0; // AVG: 1 次加法 + 1 次维护计数
//         case AGG_MIN:
//         case AGG_MAX:
//             return 1.0; // MIN/MAX: 1 次比较
//         case AGG_COMPLEX_EXPR:
//             return 5.0; // 假设复杂表达式约等于 5 次操作
//         default:
//             return 1.0; // 默认 1 次操作
//     }
// }


// void SonicHashAgg::analyze_compute_memory_ratio()
// {
//     printf("2. Compute vs Memory Bound Analysis:\n");
    
//     // 估算计算强度 (每字节内存访问的计算操作数)
//     // 对于TPC-H Q1的聚合函数：
//     double compute_operations_per_row = 0;
    
//     for (int i = 0; i < m_aggNum; i++) {
//         // 每个聚合函数的计算复杂度
//         // SUM: 1次加法
//         // AVG: 1次加法 + 维护计数  
//         // 复杂表达式: 更多计算
//         compute_operations_per_row += estimate_agg_complexity(&m_runtime->aggInfo[i]);
//     }
    
//     // 内存访问: 读取输入数据 + 更新聚合状态
//     double memory_accesses_per_row = m_aggNum * 2;  // 简化估算
    
//     double compute_intensity = compute_operations_per_row / memory_accesses_per_row;
    
//     printf("   - Compute Ops/Row: %.1f\n", compute_operations_per_row);
//     printf("   - Memory Accesses/Row: %.1f\n", memory_accesses_per_row);
//     printf("   - Compute Intensity: %.2f\n", compute_intensity);
    
//     if (compute_intensity < 1.0) {
//         printf("   🔴 MEMORY BOUND - Limited by memory bandwidth\n");
//     } else if (compute_intensity < 4.0) {
//         printf("   🟡 BALANCED - Both compute and memory matter\n");
//     } else {
//         printf("   ✅ COMPUTE BOUND - Can benefit from more threads\n");
//     }
// }


// void* thread_func_wrapper(void* arg) {
//     ThreadArg* t_arg = (ThreadArg*)arg;
//     struct timespec start, end;
//     clock_gettime(CLOCK_MONOTONIC, &start);

//     // 调用实际线程函数
//     t_arg->func(t_arg->thread_id);

//     clock_gettime(CLOCK_MONOTONIC, &end);
//     t_arg->exec_time_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
//     return nullptr;
// }

// // 返回每个线程的执行时间
// std::vector<double> SonicHashAgg::get_thread_execution_times(int num_threads)
// {
//     std::vector<ThreadArg> args(num_threads);
//     std::vector<pthread_t> threads(num_threads);

//     // 定义每个线程实际要做的工作函数，这里示例使用空函数或你实际聚合计算
//     auto worker = [](int tid) {
//         // TODO: 替换为实际工作负载
//         for (volatile int i = 0; i < 1000000; ++i); 
//     };

//     for (int i = 0; i < num_threads; ++i) {
//         args[i].thread_id = i;
//         args[i].exec_time_ns = 0;
//         args[i].func = worker;
//         pthread_create(&threads[i], nullptr, thread_func_wrapper, &args[i]);
//     }

//     for (int i = 0; i < num_threads; ++i) {
//         pthread_join(threads[i], nullptr);
//     }

//     std::vector<double> times(num_threads);
//     for (int i = 0; i < num_threads; ++i) {
//         times[i] = args[i].exec_time_ns;
//     }

//     return times;
// }

// void SonicHashAgg::analyze_workload_balance(int num_threads)
// {
//     printf("3. Workload Balance Analysis:\n");
    
//     // 获取各线程的执行时间
//     auto thread_times = get_thread_execution_times(num_threads);
//     if (thread_times.size() < 2) {
//         printf("   - Single thread mode\n");
//         return;
//     }
    
//     double avg_time = std::accumulate(thread_times.begin(), thread_times.end(), 0.0) / thread_times.size();
//     double max_time = *std::max_element(thread_times.begin(), thread_times.end());
//     double min_time = *std::min_element(thread_times.begin(), thread_times.end());
    
//     double imbalance_ratio = max_time / avg_time;
//     double efficiency = (avg_time / max_time) * 100;
    
//     printf("   - Thread Time Range: %.3fms - %.3fms\n", min_time/1e6, max_time/1e6);
//     printf("   - Load Imbalance: %.2fx\n", imbalance_ratio);
//     printf("   - Parallel Efficiency: %.1f%%\n", efficiency);
    
//     if (efficiency < 80) {
//         printf("   ⚠️ POOR LOAD BALANCE - Some threads are idle\n");
//     }
// }

// void SonicHashAgg::analyze_q1_specific_bottlenecks()
// {
//     printf("4. TPC-H Q1 Specific Analysis:\n");
    
//     // Q1特有的聚合函数模式
//     printf("   - Aggregation Functions: 8 complex aggregates\n");
//     printf("   - Data Types: Mostly double-precision FP\n");
//     printf("   - Expression Complexity: High (multiplications)\n");
    
//     // 分析向量化效果
//     analyze_vectorization_efficiency();
    
//     // 分析浮点运算吞吐量
//     analyze_fp_throughput();
// }

// void SonicHashAgg::analyze_vectorization_efficiency()
// {
//     printf("5. Vectorization Efficiency:\n");
    
//     // 估算向量化利用率
//     int vector_width = 4;  // 假设SSE/AVX宽度
//     double vectorization_efficiency = 0.0;
    
//     // 基于数据类型的向量化潜力
//     for (int i = 0; i < m_aggNum; i++) {
//         // 检查每个聚合函数是否适合向量化
//         bool can_vectorize = check_agg_vectorizable(&m_runtime->aggInfo[i]);
//         if (can_vectorize) {
//             vectorization_efficiency += 1.0 / m_aggNum;
//         }
//     }
    
//     printf("   - Vectorization Potential: %.1f%%\n", vectorization_efficiency * 100);
    
//     if (vectorization_efficiency < 0.5) {
//         printf("   ⚠️ LIMITED VECTORIZATION - Scalar operations dominate\n");
//     }
// }

// void SonicHashAgg::analyze_cache_efficiency(size_t total_groups, int agg_num) 
// {
//     printf("=== Cache Efficiency Analysis ===\n");
    
//     // 估算每个分组的内存大小
//     // 对于TPC-H Q1，每个分组存储：
//     struct AggState {
//         double sum_qty;          // 8 bytes
//         double sum_base_price;   // 8 bytes  
//         double sum_disc_price;   // 8 bytes
//         double sum_charge;       // 8 bytes
//         double avg_qty;          // 8 bytes (累计值和计数)
//         double avg_price;        // 8 bytes
//         double avg_disc;         // 8 bytes
//         int64_t count;           // 8 bytes
//         // 哈希表开销:
//         uint32_t next_ptr;       // 4 bytes (链表指针)
//         char key_storage[16];    // 16 bytes (存储分组键)
//         // 对齐填充: ~12 bytes
//         // 总计: ~100 bytes
//     };
    
//     const size_t approx_agg_state_size = 100;  // 每个分组约100字节
    
//     // 计算工作集大小
//     size_t working_set_size = total_groups * approx_agg_state_size;
    
//     // 典型的缓存大小（根据你的硬件调整）
//     size_t l1_cache_size = 32 * 1024;      // 32KB L1数据缓存
//     size_t l2_cache_size = 256 * 1024;     // 256KB L2缓存  
//     size_t l3_cache_size = 20 * 1024 * 1024; // 20MB L3缓存（服务器）
    
//     printf("Total Groups: %zu\n", total_groups);
//     printf("Working Set Size: %.2f MB\n", (double)working_set_size / (1024*1024));
//     printf("L1 Cache (32KB) Fit: %s\n", working_set_size <= l1_cache_size ? "✅ YES" : "❌ NO");
//     printf("L2 Cache (256KB) Fit: %s\n", working_set_size <= l2_cache_size ? "✅ YES" : "❌ NO");
//     printf("L3 Cache (20MB) Fit: %s\n", working_set_size <= l3_cache_size ? "✅ YES" : "❌ NO");
    
//     // 缓存行分析（64字节缓存行）
//     size_t cache_lines_needed = (working_set_size + 63) / 64;
//     size_t cache_lines_per_group = (approx_agg_state_size + 63) / 64;
    
//     printf("Cache Lines Needed: %zu\n", cache_lines_needed);
//     printf("Cache Lines per Group: %zu\n", cache_lines_per_group);
    
//     // 并行访问时的缓存竞争分析
//     if (working_set_size <= l1_cache_size) {
//         printf("Cache Status: ✅ EXCELLENT - Fits in L1 cache\n");
//     } else if (working_set_size <= l2_cache_size) {
//         printf("Cache Status: ✅ GOOD - Fits in L2 cache\n"); 
//     } else if (working_set_size <= l3_cache_size) {
//         printf("Cache Status: ⚠️ FAIR - Fits in L3 cache\n");
//     } else {
//         printf("Cache Status: ❌ POOR - Spills to main memory\n");
//     }
    
//     // 根据分组数给出建议
//     if (total_groups <= 10) {
//         printf("Recommendation: Very few groups - watch for memory contention\n");
//     } else if (total_groups <= 1000) {
//         printf("Recommendation: Good group count for parallel execution\n");
//     } else {
//         printf("Recommendation: Many groups - good for parallelism but watch cache usage\n");
//     }
//     printf("\n");
// }