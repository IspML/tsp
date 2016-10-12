#!/bin/sh
#
#
#PBS -N tsp_gr120
#PBS -o tsp_gr120.out
#PBS -e tsp_gr120.err
#PBS -m abe
#PBS -l nodes=2:ppn=8
#PBS -l walltime=10:00:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 8 PROCESSES ON 2 NODES - GR120'
echo GR120
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/gr120.txt
