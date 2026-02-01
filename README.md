# Revisiting Query Scalability of Vectorized OLAP Engines on Many-Core NUMA Servers

This repository contains the source code and experimental scripts for the paper **"Inter-Operator Interaction Matters: Revisiting Query Scalability of Vectorized OLAP Engines on Many-Core NUMA Servers"**.

## Benchmark and Baseline

We utilize **openGauss (Mixed Engine)** as the baseline system for our benchmarks. The source code is based on the openGauss 6.0.0 branch.

* **Base Repository**: [openGauss-mirror/openGauss-server](https://github.com/opengauss-mirror/openGauss-server.git)
* **Version**: openGauss 6.0.0

## Environment Configuration

To reproduce the build environment, ensure your system meets the following requirements:

### Software Requirements

* **OS**: CentOS 7.6 or openEuler 20.03 LTS (Recommended)
* **Compiler**: GCC 10.3
* **Build System**: CMake 3.25.1

### Third-Party Dependencies

The build requires `binarylibs` provided by the openGauss community.

1. Download `openGauss-third_party` binary libraries from the [openGauss Official Website](https://opengauss.org/zh/).
2. Extract the downloaded package and rename the directory to `binarylibs`.

## Compilation

The project uses `build.sh` for automated compilation and packaging.

**Command:**

```bash
sh build.sh -m [debug | release | memcheck] -3rd [binarylibs path]
```

**Parameters:**

* `-m`: Build mode. Use `release` for performance benchmarking.
* `-3rd`: Absolute path to the `binarylibs` directory.

**Example:**

```bash
[user@linux openGauss-server]$ sh build.sh -m release -3rd /opt/software/binarylibs
```

**Output:**

* **Installation Path**: `/sda/openGauss-server/dest`
* **Binary Executables**: `/sda/openGauss-server/dest/bin`

## Configuration

Modify the `data/postgresql.conf` file to configure the database engine.

Please append the following configurations to `postgresql.conf`:

```properties
port = 8888	    	# (change requires restart)
max_connections = 24000			# (change requires restart)
session_timeout = 10min			# allowed duration of any unused session, 0s-86400s(1 day), 0 is disabled 
# - Memory -
memorypool_enable = true
enable_memory_limit = true
bulk_write_ring_size = 2GB		# for bulkload, max shared_buffers
#standby_shared_buffers_fraction = 0.3 #control shared buffers use in standby, 0.1-1.0
#temp_buffers = 8MB			# min 800kB
max_prepared_transactions = 24000		# zero disables the feature
work_mem = 50GB				# min 64kB
cstore_buffers = 150GB         #min 16MB
wal_level = hot_standby			# minimal, archive, hot_standby or logical
enable_incremental_checkpoint = on	# enable incremental checkpoint
incremental_checkpoint_timeout = 60s	# range 1s-1h
#pagewriter_sleep = 100ms		# dirty page writer sleep time, 0ms - 1h
enable_double_write = on		# enable double write
max_wal_senders = 4		# max number of walsender processes
				# (change requires restart)
wal_keep_segments = 16		# in logfile segments, 16MB each normal, 1GB each in share storage mode; 0 disables
#wal_sender_timeout = 6s	# in milliseconds; 0 disables
enable_slot_log = off
max_replication_slots = 8
synchronous_standby_names = '*'	# standby servers that provide sync rep
walsender_max_send_size = 8MB  # Size of walsender max send size
#enable_data_replicate = on
# - Standby Servers -
# These settings are ignored on a master server.
hot_standby = on			# "on" allows queries during recovery
enable_kill_query = off			# optional: [on, off], default: off
logging_collector = on   		# Enable capturing of stderr and csvlog
					# into log files. Required to be on for
					# csvlogs.
					# (change requires restart)

# These are only used if logging_collector is on:
#log_directory = 'pg_log'		# directory where log files are written,
					# can be absolute or relative to PGDATA
log_filename = 'postgresql-%Y-%m-%d_%H%M%S.log'	# log file name pattern,
					# can include strftime() escapes
log_file_mode = 0600			# creation mode for log files,
					# begin with 0 to use octal notation
					# happen after that time.  0 disables.
log_rotation_size = 20MB		# Automatic rotation of logfiles will
log_min_duration_statement = 1800000	# -1 is disabled, 0 logs all statements
log_connections = off			# log connection requirement from client
log_disconnections = off		# log disconnection from client
log_duration = off			# log the execution time of each query
					# when log_duration is on and log_min_duration_statement
					# is larger than zero, log the ones whose execution time
					# is larger than this threshold
#log_error_verbosity = default		# terse, default, or verbose messages
log_hostname = off			# log hostname
log_line_prefix = '%m %u %d %h %p %S '	# special values:
log_timezone = 'PRC'
enable_alarm = on
connection_alarm_rate = 0.9
alarm_report_interval = 10
alarm_component = '/opt/snas/bin/snas_cm_cmd'
use_workload_manager = on		# Enables workload manager in the system.
					# (change requires restart)
datestyle = 'iso, mdy'
#intervalstyle = 'postgres'
timezone = 'PRC'
lc_messages = 'en_US.UTF-8'			# locale for system error message
					# strings
lc_monetary = 'en_US.UTF-8'			# locale for monetary formatting
lc_numeric = 'en_US.UTF-8'			# locale for number formatting
lc_time = 'en_US.UTF-8'				# locale for time formatting

# default configuration for text search
default_text_search_config = 'pg_catalog.english'
#deadlock_timeout = 1s
lockwait_timeout = 1200s		# Max of lockwait_timeout and deadlock_timeout + 1s
pgxc_node_name = 'db1'			# Coordinator or Datanode name
					# (change requires restart)
audit_enabled = on
enable_codegen = on			# consider use LLVM optimization
#enable_codegen_print = off		# dump the IR function
#codegen_cost_threshold = 10000		# the threshold to allow use LLVM Optimization
#------------------------------------------------------------------------------
# JOB SCHEDULER OPTIONS
#------------------------------------------------------------------------------
job_queue_processes = 10        # Number of concurrent jobs, optional: [0..1000], default: 10.
#------------------------------------------------------------------------------
# DOLPHIN OPTIONS
#------------------------------------------------------------------------------
dolphin.nulls_minimal_policy = on # the inverse of the default configuration value ! do not change !
enable_thread_pool = on
# enable_force_smp = on
# thread_pool_attr = '576,8,(cpubind: 1-20,24-44,48-68,72-92,96-119,120-143,144-167,168-191)'
# thread_pool_attr = '576,0,(nobind)'
thread_pool_stream_attr = '576,0.8,8,(cpubind: 0-23,96-119,24-47,120-143, 48-71,144-167,72-95,168-191)'
max_process_memory=500GB
enable_sonic_hashagg = on
enable_sonic_hashjoin = on
enable_bloom_filter = on
use_workload_manager=off
sql_beta_feature='sel_semi_poisson, sel_expr_instr, rand_cost_opt, page_est_opt, param_path_opt'
enable_asp=off
enable_instr_cpu_timer=off
enable_instr_rt_percentile=off
enable_instr_track_wait=off
enable_resource_track=off
enable_user_metric_persistent=off
track_activities=off
instr_unique_sql_count=0
track_sql_count=off
effective_cache_size=90GB
#max_process_memory=800GB
enable_sonic_hashagg = on
enable_sonic_hashjoin = on
# enable_bloom_filter = on
shared_buffers=100GB
work_mem=200GB
thread_pool_attr = '128,2,(nobind)'
thread_pool_stream_attr = '4096,0.2,2,(nobind)'
max_connections = 8192
explain_perf_mode=pretty
enable_sonic_hashagg = on
enable_thread_pool = on
enable_bloom_filter = on
partition_max_cache_size = 200GB
# enable_default_ustore_table = on 
enable_imcsscan =on
enable_partitionwise = on
max_imcs_cache=400GB
enable_partition_opfusion = on
enable_parallel_populate=on

```

## Instrumentation and Metrics

To analyze the performance of the distribution layer, we implemented a monitoring mechanism in `StreamProducer`.

**Source File:** `src/include/distributelayer/streamProducer.h`

**Class:** `StreamSendMonitor`

This monitor hooks into the `gs_memory_send()` function and periodically records the following metrics:

1. **Batch Count**: Total number of batches sent.
2. **Throughput**: Cumulative bytes sent.
3. **Latency**: Cumulative time spent on data copying (in microseconds).

**Log Analysis:**
The collected metrics are output to the system logs located in `/data/pg_log`. Search for entries containing monitor-specific tags to extract performance data.

## Experimental Data Availability

We provide the complete experimental datasets used in the paper within the `openGauss-server/data` directory. Readers interested in the detailed results corresponding to **Sections 4, 5, and 6** can access the raw data files there.

## Reproducibility & Benchmarking

To facilitate the reproduction of our results, we have included the TPC-H benchmarking scripts in the `openGauss-server/test` directory. Users can configure these scripts according to their local environment specifications to conduct TPC-H performance testing.

## Citation

```bibtex
@article{YourPaperTitle,
  title={Revisiting Query Scalability of Vectorized OLAP Engines on Many-Core NUMA Servers},
  author={Authors Name},
  journal={Proceedings of the VLDB Endowment},
  year={2026},
  publisher={VLDB Endowment}
}
```
