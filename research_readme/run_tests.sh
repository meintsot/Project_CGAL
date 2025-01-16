#!/bin/bash

EXE="./opt_triangulation"  # Path to your executable
INPUT_DIR="../data"  # Directory containing JSON files
OUTPUT_FILE="../data/output.json"  # Fixed output filename


# Loop through all .json files in the directory
for input_file in $(ls "$INPUT_DIR"/ortho_*.json 2>/dev/null | sort -t_ -k2,2n); do
    echo "Processing: $input_file"
    "$EXE" -i "$input_file" -o "$OUTPUT_FILE"
    printf "\n"
done

for input_file in $(ls "$INPUT_DIR"/point-set_*.json 2>/dev/null | sort -t_ -k2,2n); do
    echo "Processing: $input_file"
    "$EXE" -i "$input_file" -o "$OUTPUT_FILE"
    printf "\n"
done

for input_file in $(ls "$INPUT_DIR"/simple-polygon_*.json 2>/dev/null | sort -t_ -k2,2n); do
    echo "Processing: $input_file"
    "$EXE" -i "$input_file" -o "$OUTPUT_FILE"
    printf "\n"
done

for input_file in $(ls "$INPUT_DIR"/simple-polygon-exterior_*.json 2>/dev/null | sort -t_ -k2,2n); do
    echo "Processing: $input_file"
    "$EXE" -i "$input_file" -o "$OUTPUT_FILE"
    printf "\n"
done

for input_file in $(ls "$INPUT_DIR"/simple-polygon-exterior-*.json 2>/dev/null | sort -t_ -k2,2n); do
    echo "Processing: $input_file"
    "$EXE" -i "$input_file" -o "$OUTPUT_FILE"
    printf "\n"
done

echo "All tests completed."
