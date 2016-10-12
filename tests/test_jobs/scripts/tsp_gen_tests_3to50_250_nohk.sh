#!/bin/sh
#
#
#PBS -N tsp_gen_tests_3to50_250_nohk
#PBS -o tsp_gen_tests_3to50_250_nohk.out
#PBS -e tsp_gen_tests_3to50_250_nohk.err
#PBS -m abe
#PBS -l nodes=2:ppn=8
#PBS -l walltime=8:00:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 8 PROCESSES ON 2 NODES - RANDOM DISTANCES BETWEEN 1 AND 250 - NO HELD-KARP'
for i in {3..50}
do
	echo $i CITIES
	mympirun $VSC_HOME/TSP/tsp_no_hk.x $VSC_HOME/TSP/test_gen/$i.txt
done
