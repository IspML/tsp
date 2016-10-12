#!/bin/sh
#
#
#PBS -N tsp_gen_tests_3to50_250_nosplit
#PBS -o tsp_gen_tests_3to50_250_nosplit.out
#PBS -e tsp_gen_tests_3to50_250_nosplit.err
#PBS -m abe
#PBS -l nodes=2:ppn=8
#PBS -l walltime=3:50:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 8 PROCESSES ON 2 NODES - RANDOM DISTANCES BETWEEN 1 AND 250 - SPLIT DEPTH TESTS'
for i in {3..50}
do
	j=1
	while [ $j -le $i ]
	do
		echo $i CITIES WITH SPLIT DEPTH $j
		mympirun $VSC_HOME/TSP/tsp_no_split.x $VSC_HOME/TSP/test_gen/$i.txt $j
		let j++
	done
done
