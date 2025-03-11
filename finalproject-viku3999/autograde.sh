#!/bin/bash

### Build your executable
make

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NOCOLOR='\033[0m'

EXEC="./containers"
chmod u+x $EXEC

# Default directory for tests
TESTDIR="./autograde_tests"
TESTRESULT="./autograde_results"
ANALYSIS=false

# Output CSV file for Excel (only if analysis is true)
OUTPUT_CSV="test_results.csv"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -a)
            ANALYSIS=true
            shift
            ;;
        *)
            if [ ! -z "$1" ]; then
                TESTDIR=$1
            fi
            shift
            ;;
    esac
done

# Exit if no directory provided and the default one doesn't work
if [ ! -d "$TESTDIR" ]; then
    echo -e "${RED}Invalid or no input directory provided. Exiting script.${NOCOLOR}"
    exit 1
fi

SCORE=0

# Function to measure throughput and round off to 3 decimal places
measure_throughput() {
    local num_count=$1
    local start_time=$2
    local end_time=$3
    local throughput=$(echo "$num_count / ($end_time - $start_time)" | bc -l)
    # Round to 3 decimal places
    printf "%.3f\n" $throughput
}

# Function to measure and print runtime
measure_runtime() {
    local start_time=$1
    local end_time=$2
    local runtime=$(echo "$end_time - $start_time" | bc -l)
    printf "%.3f\n" $runtime
}

# Only initialize CSV if -analysis flag is given
if $ANALYSIS; then
    echo "Algorithm,Elements,Threads,Throughput (elements/sec),Runtime (seconds),Result" > $OUTPUT_CSV
fi

# Queue algorithm
for prim in "--alg=sgl_queue" "--alg=sgl_queue_flat" "--alg=ms"; do
    if $ANALYSIS; then
        for NUM_COUNT in "1000000"; do
            for THREADS in {1..20}; do
                ANS=$TESTDIR/$NUM_COUNT-queue.sol
                MY=$TESTRESULT/$NUM_COUNT$prim.my
                rm $MY

                # Measure start time
                start_time=$(date +%s.%N)

                $EXEC -n $NUM_COUNT -o $MY -t $THREADS $prim

                # Measure end time
                end_time=$(date +%s.%N)

                # Measure throughput
                throughput=$(measure_throughput $NUM_COUNT $start_time $end_time)

                # Measure and print runtime
                runtime=$(measure_runtime $start_time $end_time)

                # Record results in CSV file if analysis is enabled
                if cmp --silent $MY $ANS;
                then 
                    SCORE=$(($SCORE+5))
                    result="${GREEN}Pass${NOCOLOR}"
                else 
                    result="${RED}FAIL${NOCOLOR}"
                fi

                # Output analysis details on console if analysis is true
                if $ANALYSIS; then
                    echo -e "$prim for $NUM_COUNT elements with $THREADS threads:"
                    echo -e "Time taken: ${YELLOW}$runtime seconds${NOCOLOR}"
                    echo -e "Throughput: ${YELLOW}$throughput elements/sec${NOCOLOR}"
                fi

                # Write to CSV if analysis is enabled
                if $ANALYSIS; then
                    echo "$prim,$NUM_COUNT,$THREADS,$throughput,$runtime,$result" >> $OUTPUT_CSV
                fi

                # Print formatted output to the terminal
                echo -e "$NUM_COUNT elements, $prim with $THREADS threads ..... $result"
            done
        done
    else
        for NUM_COUNT in "100" "500" "1000" "5000" "10000"; do
            ANS=$TESTDIR/$NUM_COUNT-queue.sol
            MY=$TESTRESULT/$NUM_COUNT$prim.my
            rm $MY

            # Measure start time
            start_time=$(date +%s.%N)

            $EXEC -n $NUM_COUNT -o $MY -t $THREADS $prim

            # Measure end time
            end_time=$(date +%s.%N)

            # Measure throughput
            throughput=$(measure_throughput $NUM_COUNT $start_time $end_time)

            # Measure and print runtime
            runtime=$(measure_runtime $start_time $end_time)

            # Print results without CSV file if analysis is not enabled
            if cmp --silent $MY $ANS;
            then 
                SCORE=$(($SCORE+5))
                result="${GREEN}Pass${NOCOLOR}"
            else 
                result="${RED}FAIL${NOCOLOR}"
            fi

            # Print formatted output to the terminal
            echo -e "$NUM_COUNT elements, $prim ..... $result"
        done
    fi
done

# Stack algorithm
for prim in "--alg=sgl_stack" "--alg=sgl_stack_elem" "--alg=sgl_stack_flat" "--alg=treiber" "--alg=treiber_elem"; do
    if $ANALYSIS; then
        for NUM_COUNT in "1000000"; do
            for THREADS in {1..20}; do
                ANS=$TESTDIR/$NUM_COUNT-stack.sol
                MY=$TESTRESULT/$NUM_COUNT$prim.my
                rm $MY

                # Measure start time
                start_time=$(date +%s.%N)

                $EXEC -n $NUM_COUNT -o $MY -t $THREADS $prim

                # Measure end time
                end_time=$(date +%s.%N)

                # Measure throughput
                throughput=$(measure_throughput $NUM_COUNT $start_time $end_time)

                # Measure and print runtime
                runtime=$(measure_runtime $start_time $end_time)

                # Record results in CSV file if analysis is enabled
                if cmp --silent $MY $ANS;
                then 
                    SCORE=$(($SCORE+5))
                    result="${GREEN}Pass${NOCOLOR}"
                else 
                    result="${RED}FAIL${NOCOLOR}"
                fi

                # Output analysis details on console if analysis is true
                if $ANALYSIS; then
                    echo -e "$prim for $NUM_COUNT elements with $THREADS threads:"
                    echo -e "Time taken: ${YELLOW}$runtime seconds${NOCOLOR}"
                    echo -e "Throughput: ${YELLOW}$throughput elements/sec${NOCOLOR}"
                fi

                if $ANALYSIS; then
                    echo "$prim,$NUM_COUNT,$THREADS,$throughput,$runtime,$result" >> $OUTPUT_CSV
                fi

                # Print formatted output to the terminal
                echo -e "$NUM_COUNT elements, $prim with $THREADS threads ..... $result"
            done
        done
    else
        for NUM_COUNT in "100" "500" "1000" "5000" "10000"; do
            ANS=$TESTDIR/$NUM_COUNT-stack.sol
            MY=$TESTRESULT/$NUM_COUNT$prim.my
            rm $MY

            # Measure start time
            start_time=$(date +%s.%N)

            $EXEC -n $NUM_COUNT -o $MY -t $THREADS $prim

            # Measure end time
            end_time=$(date +%s.%N)

            # Measure throughput
            throughput=$(measure_throughput $NUM_COUNT $start_time $end_time)

            # Measure and print runtime
            runtime=$(measure_runtime $start_time $end_time)

            # Print results without CSV file if analysis is not enabled
            if cmp --silent $MY $ANS;
            then 
                SCORE=$(($SCORE+5))
                result="${GREEN}Pass${NOCOLOR}"
            else 
                result="${RED}FAIL${NOCOLOR}"
            fi

            # Print formatted output to the terminal
            echo -e "$NUM_COUNT elements, $prim ..... $result"
        done
    fi
done

# Print the final score
echo -e "${YELLOW}Score: $SCORE${NOCOLOR}"

RET=1
if [ $SCORE == "40" ]
then
    RET=0
fi

exit $RET
