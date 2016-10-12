#!/bin/sh
#
#
#PBS -N tsp_gen_tests_3to50_MAX
#PBS -o tsp_gen_tests_3to50_MAX.out
#PBS -e tsp_gen_tests_3to50_MAX.err
#PBS -m abe
#PBS -l nodes=2:ppn=8
#PBS -l walltime=0:50:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 8 PROCESSES ON 2 NODES - RANDOM DISTANCES BETWEEN 1 AND INT_MAX/amount_of_cities'
for i in {3..50}
do
	echo $i CITIES
	mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/test_gen_MAX/$i.txt
done
