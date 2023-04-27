#!/bin/bash

cd images
# Rename all *.jpg to *
for file in *.jpg; do
    mv -- "$file" "${file%.jpg}"
done

