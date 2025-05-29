#!/bin/bash

# Step 1: Compile final.cpp with C++17
g++ -std=c++17 -O2 -o part_b part_b.cpp

# Step 2: Run the compiled program with input.txt and save output to temp.txt
./part_b 

rm part_b out.txt