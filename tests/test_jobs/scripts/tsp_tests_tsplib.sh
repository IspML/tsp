#!/bin/sh
#
#
#PBS -N tsp_tests_tsplib
#PBS -o tsp_tests_tsplib.out
#PBS -e tsp_tests_tsplib.err
#PBS -m abe
#PBS -l nodes=2:ppn=8
#PBS -l walltime=1:30:00
#

module load ictce
module load VSC-tools

echo 'TEST WITH 8 PROCESSES ON 2 NODES - TSPLIB'
echo P01 -NOT TSPLIB-
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/p01_not_tsplib.txt
echo GR17
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/gr17.txt
echo GR21
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/gr21.txt
echo ULYSSES22
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/ulysses22.txt
echo FRI26
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/fri26.txt
echo BAYG29
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/bayg29.txt
echo BAYS29
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/bays29.txt
echo DANTZIG42
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/dantzig42.txt
echo SWISS42
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/swiss42.txt
echo GR48
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/gr48.txt
echo HK48
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/hk48.txt
echo BRAZIL58
mympirun $VSC_HOME/TSP/tsp.x $VSC_HOME/TSP/tests_tsplib/brazil58.txt
