#!/bin/bash
ALGO_BINARY=$1
PREFIX=$2
OUTPUT_NAME=$3

# Check if ALGO directory exists
if [ ! -d "output/$OUTPUT_NAME" ]; then
    echo "Directory output/$OUTPUT_NAME does not exist."
    exit 1
fi

# Write header to output/[ALGO]_merged.csv
echo "$PREFIX,$(build/cli/$ALGO_BINARY /dev/null --onlyheader)" > "output/${OUTPUT_NAME}_merged.csv"

# Concatenate files
for file in "output/$OUTPUT_NAME"/*.csv; do
    cat "$file" >> "output/${OUTPUT_NAME}_merged.csv"
done
