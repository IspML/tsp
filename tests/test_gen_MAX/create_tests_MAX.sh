#!/bin/bash

random=5
for i in {3..100}
do
	./tsp_gen $i $(($random + $i)) > $i.txt
done
