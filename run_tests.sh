#!/bin/bash

EXE="./build/opt_triangulation"  # Path to your executable
INPUT_DIR="./data"  # Directory containing JSON files
OUTPUT_FILE="./data/output.json"  # Fixed output filename


# Loop through all .json files in the directory
for input_file in "$INPUT_DIR"/ortho*.json; do
    echo "Processing: $input_file"
    "$EXE" -i "$input_file" -o "$OUTPUT_FILE"
    printf "\n"
done

echo "All tests completed."
