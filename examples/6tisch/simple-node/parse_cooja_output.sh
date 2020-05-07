#!/bin/bash

# Cooja energest output parser to CSV.
# Author: Andreas Baroutsos<bardoutsos@ceid.upatras.gr>
# Wireless Sensor Networks 2019-2020

# Instructions
# Execute the script with argument the cooja output.
# e.g. parse_cooja_output.sh rpl_simulation.txt

temp="temp.txt" 
energest_log="energest_log.csv"

if [ "$#" -ne 1 ]; then
    echo "You must specify the cooja log file"
else
    cooja_log=$1
    echo "id;count;total_time;CPU;LPM;DeepLPM;Radio_TX;Radio_RX;Radio_Total" > $energest_log
    if [ -f $cooja_log ]; then
        echo cooja_log : $cooja_log
        sed -n '/Energest/p' $cooja_log > $temp
        awk -F"[;:\t ]" 'BEGIN {OFS=";"} {print $4,$10,$11,$12,$13,$14,$15,$16,$17}' $temp >> $energest_log
        echo "CSV of energest output : "$(pwd)/$energest_log
        rm $temp
    else
        echo INVALID input.
    fi
fi
