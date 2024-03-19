
#!/bin/bash
#! /bin/usr/env bash
# Bash script to execute single bottleneck topology and parking lot topology 
# Date Created : 21/Sept/2023
# Last Modified Date : 02/March/2023
# Last Execution Date : 02/March/2023

declare -a tcp_flavor=("TcpNewReno" "TcpLinuxReno" "TcpCubic")
declare -a round_trip_time
for k in {0..58}
do 
    round_trip_time[$k]=$((10 + k * 5 - 6))ms
done
declare -a queue_size=(1250.4 100 15);


for a in "${round_trip_time[@]}"
do 
    echo $a
done

for i in "${queue_size[@]}"
do
    for j in "${round_trip_time[@]}"
    do
        ./ns3 run "scratch/trial02.cc --flavour=${tcp_flavor[0]} --RTT=$j --R6_queue_size=$i";           
    done
done
