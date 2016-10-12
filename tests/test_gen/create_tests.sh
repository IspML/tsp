#!/bin/bash

random=5
size=250
for i in {3..100}
do
	./tsp_gen $i $(($random + $i)) $size > $i.txt
done
