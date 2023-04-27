#!/bin/bash

for file in '*.c'; do
	mkdir handin
	find . -type f -name "*.c" -exec cp {} handin \;
	zip -r handin.zip handin
	rm -r -v handin
done
