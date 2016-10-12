#!/bin/sh
#
#
#PBS -N tsp_gen_tests_51to75_250
#PBS -o tsp_gen_tests_51to75_250.out
#PBS -e tsp_gen_tests_51to75_250.err
#PBS -m abe
#PBS -l nodes=2:ppn=8
#PBS -l walltime=0:50:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 8 PROCESSES ON 2 NODES - RANDOM DISTANCES BETWEEN 1 AND 250'
for i in {51..75}
do
	echo $i CITIES
	mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/test_gen/$i.txt
done
