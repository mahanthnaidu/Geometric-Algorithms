#!/bin/bash
echo "Give number of points followed by a point in each point represented by space seperated numbers"
read -p "Press Enter to continue"
# Step 1: Compile final.cpp with C++17
g++ -std=c++17 -O2 -o part_a part_a.cpp

# Step 2: Run the compiled program with input.txt and save output to temp.txt
./part_a < input.txt > temp.txt

# Step 3: Append the output (temp.txt) to the end of input.txt
cat temp.txt >> input.txt
rm temp.txt part_a

# Step 4: Run the Python plotting script
python3 plot.py