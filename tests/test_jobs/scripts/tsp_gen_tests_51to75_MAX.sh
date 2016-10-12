#!/bin/sh
#
#
#PBS -N tsp_gen_tests_51to75_MAX
#PBS -o tsp_gen_tests_51to75_MAX.out
#PBS -e tsp_gen_tests_51to75_MAX.err
#PBS -m abe
#PBS -l nodes=2:ppn=8
#PBS -l walltime=1:00:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 8 PROCESSES ON 2 NODES - RANDOM DISTANCES BETWEEN 1 AND INT_MAX/AMOUNT_OF_CITIES'
for i in {51..75}
do
	echo $i CITIES
	mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/test_gen_MAX/$i.txt
done
