#!/bin/sh
#
#
#PBS -N tsp_gen_tests_3to50_250_one
#PBS -o tsp_gen_tests_3to50_250_one.out
#PBS -e tsp_gen_tests_3to50_250_one.err
#PBS -m abe
#PBS -l nodes=1:ppn=1
#PBS -l walltime=1:50:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 1 PROCESS - RANDOM DISTANCES BETWEEN 1 AND 250'
for i in {3..50}
do
	echo $i CITIES
	mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/test_gen/$i.txt
done
