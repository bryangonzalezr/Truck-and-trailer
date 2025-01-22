#!/bin/bash

# Define the directory containing the instance files
INSTANCE_DIR="instancesMedium"

# Define the output file name
OUTPUT_FILE="resultsBigLunes.txt"

# Number of iterations per run
ITERATIONS=1000

# Number of runs per solution
NUM_RUNS=30

# Seed
SEED=2024

# Solution Folder
SOLUTION_FOLDER="sols_testLunes"

# Remove the existing output file
rm -f "$OUTPUT_FILE"

# Define solution types
SOLUTION_TYPES=("SolutionB" "SolutionC" "SolutionR")


CSV_FILE="execution_summary.csv"
echo "Instance,Total Time (seconds)" > "$CSV_FILE"

# Iterate over each instance file in the directory

# Create the solution folder if it does not exist
mkdir -p "$SOLUTION_FOLDER"

for instance_file in $INSTANCE_DIR/*.dat; do
    # Extract the instance name from the file path
    instance_name=$(basename "$instance_file" .dat)

    # Create instance directory
    instance_dir="$SOLUTION_FOLDER/$instance_name"
    mkdir -p "$instance_dir"

    # Initialize total time for this instance
    total_instance_time=0

    # Run each solution type
    for solution_type in "${SOLUTION_TYPES[@]}"; do
        # Create solution type directory inside instance directory
        solution_dir="$instance_dir/$solution_type"
        mkdir -p "$solution_dir"

        for ((run=1; run<=NUM_RUNS; run++)); do
            current_seed=$((run*1000000))
            # Run the C++ program with the instance file as an argument
            echo "Running instance: $instance_name with $solution_type, run $run, seed $current_seed"
            execution_output=$("./$solution_type" "$instance_file" $ITERATIONS $solution_type $run $SOLUTION_FOLDER $current_seed)
            
            # Extract the execution time and total distance from the execution output
            execution_time=$(echo "$execution_output" | grep -oP 'Execution time:\s*\K\d+')
            total_distance=$(echo "$execution_output" | grep -oP 'Total distance:\s*\K\d+\.\d+')

            # Accumulate execution time for the instance
            total_instance_time=$((total_instance_time + execution_time))

            # Export the results to the output file
            echo "Instance: $instance_name, Solution: $solution_type, Run: $run, Seed: $current_seed, Execution time: $execution_time ms, Total distance: $total_distance" >> "$OUTPUT_FILE"
        done
    done

    # Convert total time to seconds
    total_instance_time_seconds=$(echo "scale=3; $total_instance_time / 1000" | bc)

    # Export the total time in seconds for this instance to the CSV file
    echo "$instance_name,$total_instance_time_seconds" >> "$CSV_FILE"
done

echo "Results exported to $OUTPUT_FILE"