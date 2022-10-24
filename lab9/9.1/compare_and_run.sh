#!/bin/bash

g++ main.cpp -o main

rm out.txt

./main 0 12345 12 750 & 
./main 1 12345 15 150 & 
./main 2 12345 9 1332 & 
