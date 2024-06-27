#!/bin/bash

# Define the directory containing the instance files
INSTANCE_DIR="instancesMedium"

# Define the output file name
OUTPUT_FILE="resultsMedium.txt"

ITERATIONS=10000

# Remove the existing output file
rm -f "$OUTPUT_FILE"

# Iterate over each instance file in the directory
for instance_file in $INSTANCE_DIR/*.dat; do
    # Extract the instance name from the file path
    instance_name=$(basename "$instance_file" .dat)

    # Run the C++ program with the instance file as an argument
    echo "Running instance: $instance_name"
    execution_output=$(./ttrp_sparse "$instance_file" $ITERATIONS)

    # Extract the execution time and total distance from the execution output
    execution_time=$(echo "$execution_output" | grep -oP 'Execution time:\K\d+')
    total_distance=$(echo "$execution_output" | grep -oP 'Total distance:\K\d+\.\d+')

    # Export the results to the output file
    echo "Instance: $instance_name, Execution time: $execution_time ms, Total distance: $total_distance" >> "$OUTPUT_FILE"
done

echo "Results exported to $OUTPUT_FILE"