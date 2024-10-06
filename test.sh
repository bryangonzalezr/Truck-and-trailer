#!/bin/bash

# Define the directory containing the instance files
INSTANCE_DIR="instances"

# Define the output file name
OUTPUT_FILE="resultsBig.txt"

# Number of iterations per run
ITERATIONS=1000

# Number of runs per solution
NUM_RUNS=10

# Seed
SEED=1

# Solution Folder
SOLUTION_FOLDER="sols_test"

# Remove the existing output file
rm -f "$OUTPUT_FILE"

# Define solution types
SOLUTION_TYPES=("SolutionB" "SolutionC" "SolutionR")



# Iterate over each instance file in the directory

# Create the solution folder if it does not exist
mkdir -p "$SOLUTION_FOLDER"

for instance_file in $INSTANCE_DIR/*.txt; do
    # Extract the instance name from the file path
    instance_name=$(basename "$instance_file" .txt)

    # Create instance directory
    instance_dir="$SOLUTION_FOLDER/$instance_name"
    mkdir -p "$instance_dir"

    # Run each solution type
    for solution_type in "${SOLUTION_TYPES[@]}"; do
        # Create solution type directory inside instance directory
        solution_dir="$instance_dir/$solution_type"
        mkdir -p "$solution_dir"

        for ((run=1; run<=NUM_RUNS; run++)); do
            # Run the C++ program with the instance file as an argument
            echo "Running instance: $instance_name with $solution_type, run $run"
            execution_output=$("./$solution_type" "$instance_file" $ITERATIONS $solution_type $run $SOLUTION_FOLDER $SEED)

            # Extract the execution time and total distance from the execution output
            execution_time=$(echo "$execution_output" | grep -oP 'Execution time:\K\d+')
            total_distance=$(echo "$execution_output" | grep -oP 'Total distance:\K\d+\.\d+')

            # Export the results to the output file
            echo "Instance: $instance_name, Solution: $solution_type, Run: $run, Execution time: $execution_time ms, Total distance: $total_distance" >> "$OUTPUT_FILE"
        done
    done
done

echo "Results exported to $OUTPUT_FILE"
