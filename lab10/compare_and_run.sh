#!/bin/bash

g++ writer.cpp -o writer
g++ reader.cpp -o reader

rm ./out.txt

SEM_KEY=12345;

index_r=1; 
index_w=0;

while (( $index_w < 5 )) # writers
do
	./writer $index_w $SEM_KEY &
	let "index_w=index_w+1"
done

while (( $index_r < 15 )) # readers
do
	./reader -$index_r $SEM_KEY &
	let "index_r=index_r+1"
done
