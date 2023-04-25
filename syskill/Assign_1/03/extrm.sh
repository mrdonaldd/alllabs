#!/bin/bash

# Rename all *.jpg to *
for file in *.jpg; do
    mv -- "$file" "${file%.jpg}"
done

