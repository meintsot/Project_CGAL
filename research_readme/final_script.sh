#!/bin/bash

EXE="./opt_triangulation"  # Path to your executable
INPUT_DIR="../data"  # Directory containing input JSON files
OUTPUT_DIR="../results"  # Directory to store output files

# Create the output directory if it doesn't exist
#mkdir -p "$OUTPUT_DIR"

# Loop through only input JSON files
for input_file in $(ls "$INPUT_DIR"/simple-polygon-exterior-*.json 2>/dev/null | sort -t_ -k2,2n); do
    # Extract filename without extension
    base_name=$(basename "$input_file" .json)
    
    # Define output file path in the OUTPUT_DIR
    output_file="$OUTPUT_DIR/${base_name}.output.json"
    
    echo "Processing: $input_file -> $output_file"
    "$EXE" -i "$input_file" -o "$output_file"

    printf "\n"
done
