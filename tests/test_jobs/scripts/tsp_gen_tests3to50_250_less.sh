#!/bin/sh
#
#
#PBS -N tsp_gen_tests_3to50_250_less
#PBS -o tsp_gen_tests_3to50_250_less.out
#PBS -e tsp_gen_tests_3to50_250_less.err
#PBS -m abe
#PBS -l nodes=1:ppn=4
#PBS -l walltime=0:50:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 4 PROCESSES ON 1 NODE - RANDOM DISTANCES BETWEEN 1 AND 250'
for i in {3..50}
do
	echo $i CITIES
	mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/test_gen/$i.txt
done
