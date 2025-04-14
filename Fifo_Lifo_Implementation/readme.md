# Assignment 3: FIFO and LRU Page Replacement Algorithms

## Overview
This assignment implements two page replacement algorithms:
- **FIFO (First In First Out)**
- **LRU (Least Recently Used)**

The goal of this program is to simulate the behavior of these algorithms and compare the differences.

## Requirements
- **Compiler**: g++ with support for C++20 standard.
  
## How to Compile
1. To compile the code, run the following command in the terminal:
   ```bash
   make fifo/lru

This will generate the following executable files:

fifo: Simulates FIFO page replacement.

lru: Simulates LRU page replacement.

## How to run
Run fifo using:
   ```bash
   fifo -M [mainmemory] -V [swapspace] -P [pagesize] -i [infile] -o [outfile]
```
Run lru using:
   ```bash
   lru -M [mainmemory] -V [swapspace] -P [pagesize] -i [infile] -o [outfile]
```
Remove executables using:
   ```bash
   make clean
   ```

## Error cases and weaknesses
1. The program won't work if the input is not formatted as expected.

2. make clean uses del /f which would not work on Linux

## Sources
1. Syntax for reading input from the command line has been referred from the internet.

2. Syntax for fstream and stringstream has also been looked up.


