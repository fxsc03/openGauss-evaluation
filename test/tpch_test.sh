# VALUES=(96 128)
VALUES=(128)
# VALUES=(8 16 32 64 96 128)
VALUES2=(5) 
# VALUES2=(7 10)
# k is the SQL query ID, i is the degree of parallelism (DOP)
for k in "${VALUES2[@]}"
do
      
    # Loop through the degree of parallelism values
    for i in "${VALUES[@]}"
    do
        # Restart the database
        gs_ctl restart -D /your_dir/data -Z single_node  | grep -v "LOG: " > /dev/null

        temp_file="temp.sql"
        echo "\timing on" > temp.sql
        echo -e "set query_dop = $i;" >> temp.sql  # Set DOP in the temporary file
        # Output the DOP setting
        # Loop to execute the SQL query multiple times
        # for j in {1..7}
        for j in {1..1}
        do
            # Execute external SQL file via gsql, passing the DOP setting
            # echo "Executing SQL with DOP=$i (iteration $j)"
            # echo -e "set query_dop = $i;" > temp.sql  # Set DOP in the temporary file
            #cat query.sql >> temp.sql  # Append external SQL query file content to the temp file
            # cat /mnt/yyz/TPCH-openGauss-TestTools/TPCH-openGauss-TestTools/SQL/SQL/$k.sql >> temp.sql
            
            # cat /home/fxs/SQL/$k.sql >> temp.sql

            cat /your_dir/SQL/$k.sql >> temp.sql
            
            # cat /home/fxs/TPCH-openGauss-TestTools/SQL/SQL/$k.sql >> temp.sql
            # Execute the merged SQL file
            # gsql -p 9999 -d tpch_db -f temp.sql
        done

        # cat /home/fxs/SQL/$k.sql >> temp.sql


        echo -e "SQL ID is $k, DOP is $i \n"
        
        # Find the database server process PID (gaussdb)
        # Note: To monitor all database threads (including worker threads), use:
        # perf stat -e context-switches,cpu-migrations -t $(pgrep -P $DB_PID | tr '\n' ',' | sed 's/,$//'),$DB_PID



        # DB_PID=$(pgrep -f "gaussdb.*-D.*data" | head -1)
        # if [ -z "$DB_PID" ]; then
        #     # If not found, try another way to locate it
        #     DB_PID=$(ps aux | grep "[g]aussdb.*-D.*data" | awk '{print $2}' | head -1)
        # fi
        
        # if [ -z "$DB_PID" ]; then
        #     echo "Warning: Unable to find database server process, monitoring all gaussdb processes"
        #     # Monitor all gaussdb processes
        #     DB_PIDS=$(pgrep gaussdb | tr '\n' ',' | sed 's/,$//')
        #     if [ -n "$DB_PIDS" ]; then
        #         # Start perf monitoring in the background and execute SQL simultaneously
        #         perf stat -e context-switches,cpu-migrations -p $DB_PIDS &
        #         PERF_PID=$!
        gsql -p your_port -d tpch_db -f temp.sql -q
        #         # Stop perf monitoring
        #         kill -INT $PERF_PID 2>/dev/null || true
        #         wait $PERF_PID 2>/dev/null || true
        #     else
        #         echo "Error: No gaussdb processes found"
        #         gsql -p 8888 -d tpch_db -f temp.sql -q
        #     fi
        # else
        #     echo "Monitoring database process PID: $DB_PID"
        #     # Start perf monitoring for the database process in the background
        #     perf stat -e context-switches,cpu-migrations -p $DB_PID &
        #     PERF_PID=$!
            
        #     # Execute SQL query
        #     gsql -p 8888 -d tpch_db -f temp.sql -q
            
        #     # Stop perf monitoring (send SIGINT signal)
        #     kill -INT $PERF_PID 2>/dev/null || true
        #     wait $PERF_PID 2>/dev/null || true
        # fi

    done
done