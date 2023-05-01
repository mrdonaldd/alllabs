#!/bin/bash

#read -r -p 'Enter pattern: ' pattern

result=$( grep -e $1 guest.csv )

if [ -n "$result" ]; then
    printf '%s\n' "$result"
else
    printf 'No match found for pattern "%s"\n' "$pattern"
fi
