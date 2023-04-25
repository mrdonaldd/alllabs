#!/bin/bash
for i in *.txt; do 
echo "$i : $(sed -n '10p' "$i")";
done

# or
# awk 'FNR==10 {print FILENAME, $0}' *.txt