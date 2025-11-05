#!/bin/sh
for i in $(seq 1 1000); do
	./qsort_large input_large.dat > output_large.txt
done
