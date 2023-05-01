#!/bin/bash

result=$(grep -e $1 guest.csv)
if [ -n "$result" ]; then
    printf '%s\n' "$result"
else
    printf 'No match found'
fi
