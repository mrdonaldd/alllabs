#!/bin/bash

cd $2
# Rename all *.jpg to *
for file in *.$1; do
    mv -- "$file" "${file%.jpg}"
done

