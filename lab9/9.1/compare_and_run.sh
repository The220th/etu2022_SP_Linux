#!/bin/bash

g++ main.cpp -o main

rm out.txt

./main 0 12345 25 750 & 
./main 1 12345 123 150 & 
./main 2 12345 81 1332 & 
