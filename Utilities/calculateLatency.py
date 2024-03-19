import pandas as pd 
import numpy as np
import math


#-----------------------------------------------------------------
#               Calculating Queueing Delay(ms)
#        Queueing Delay = (QueueSize * 1446 * 8) / (Router Capacity * 10^6) * 10^3
#-----------------------------------------------------------------
def calculate_QueueingDelay(f1, QueueType, RTT, Metric):
    file = pd.read_csv(f1, delimiter='\t', header=None)
    filename = "./NewResults/TCPNewReno/" + QueueType + "/" + QueueType +  RTT + "RTT" + Metric + ".txt" 
    QD = open(filename, "a")
    
    R1 = file[1]
    R2 = file[2]
    R3 = file[3]
    R4 = file[4]
    R5 = file[5]
    R6 = file[6]
    columns = 6
    queueing_delay = np.zeros(columns)
    for i in range(200,250):
        QD.write(str(i) + "\t")
        queueing_delay[0] = round(((R1[i] * 1446 * 8)/ (100 * 1000000)) * 1000, 3)
        QD.write(str(queueing_delay[0]) + "\t")
        queueing_delay[1] = round( ((R2[i]* 1446 * 8)/ (40 * 1000000)) * 1000, 3)
        QD.write(str(queueing_delay[1]) + "\t")
        queueing_delay[2] = round( ((R3[i]* 1446 * 8)/ (40 * 1000000)) * 1000, 3)
        QD.write(str(queueing_delay[2]) + "\t")
        queueing_delay[3] = round( ((R4[i]* 1446 * 8)/ (60 * 1000000)) * 1000, 3)
        QD.write(str(queueing_delay[3]) + "\t")
        queueing_delay[4] = round(((R5[i]* 1446 * 8)/ (60 * 1000000)) * 1000, 3)
        QD.write(str(queueing_delay[4]) + "\t")
        queueing_delay[5] = round(((R6[i]* 1446 * 8)/ (60 * 1000000)) * 1000, 3)
        QD.write(str(queueing_delay[5]) + "\n")

#-----------------------------------------------------------------
#                   Calculating Latency 
#        Latency = RTT + Queueing Delay per route enroute 
#-----------------------------------------------------------------
def calculate_latency(f2, QueueType, str_RTT, RTT, Metric):
    file = pd.read_csv(f2, delimiter='\t', header=None)
    filename = "./NewResults/TCPNewReno/" + QueueType + "/" + QueueType + str_RTT + "RTT" + Metric + ".txt" 
    QD = open(filename, "a")
    
    R1 = file[1]
    R2 = file[2]
    R3 = file[3]
    R4 = file[4]
    R5 = file[5]
    R6 = file[6]
    columns = 8

    latency = np.zeros(columns)
    for i in range(0,50):
        QD.write(str(i+200) + "\t")
        latency[0] = round(RTT + R1[i] + R2[i], 3)
        QD.write(str(latency[0]) + "\t")
        latency[1] = round(RTT + R1[i] + R2[i] + R4[i] + R6[i], 3)
        QD.write(str(latency[1]) + "\t")
        latency[2] = round(RTT + R1[i] + R2[i] + R6[i], 3)
        QD.write(str(latency[2]) + "\t")
        latency[3] = round(RTT + R1[i] + R3[i] + R4[i] + R6[i], 3)
        QD.write(str(latency[3]) + "\t")
        latency[4] = round(RTT + R1[i] + R3[i] + R4[i] + R5[i], 3)
        QD.write(str(latency[4]) + "\t")
        latency[5] = round(RTT + R1[i] + R3[i] + R4[i] + R5[i] + R6[i], 3)
        QD.write(str(latency[5]) + "\t")
        latency[6] = round(RTT + R1[i] + R3[i] + R6[i], 3)
        QD.write(str(latency[6]) + "\t")
        latency[7] = round(RTT + R1[i] + R3[i], 3)
        QD.write(str(latency[7]) + "\n")
        
#-----------------------------------------------------------------
# Calculating Mean and Standard Deviation for latency
#-----------------------------------------------------------------
def find_mean_SDV(f2, QueueType, RTT, Metric):
    file = pd.read_csv(f2, delimiter='\t', header=None)
    filename = "./NewResults/TCPNewReno/Statistics/" + QueueType + "_Mean_" + Metric + ".txt" 
    M = open(filename, "a")
    filename = "./NewResults/TCPNewReno/Statistics/" + QueueType + "_SD_" + Metric + ".txt" 
    SD = open(filename, "a")
    
    R1 = file[1]
    R2 = file[2]
    R3 = file[3]
    R4 = file[4]
    R5 = file[5]
    R6 = file[6]
    R7 = file[7]
    R8 = file[8]
    columns = 8
    total = np.zeros(columns)
    for i in range(0,50):
        total[0] = total[0] + R1[i]
        total[1] = total[1] + R2[i]
        total[2] = total[2] + R3[i]
        total[3] = total[3] + R4[i]
        total[4] = total[4] + R5[i]
        total[5] = total[5] + R6[i]
        total[6] = total[6] + R7[i]
        total[7] = total[7] + R8[i]

    M.write(RTT + "\t")
    average = np.zeros(columns)
    for i in range(0,columns):
        average[i] = round(total[i] / 50, 3)
        M.write(str(average[i]) + "\t")
    M.write("\n")

    deviation = np.zeros(columns)
    standard_deviation = np.zeros(columns)
    for i in range(0,50):
        deviation[0] = deviation[0] + ((R1[i] - average[0])**2)
        deviation[1] = deviation[1] + ((R2[i] - average[1])**2)
        deviation[2] = deviation[2] + ((R3[i] - average[2])**2)
        deviation[3] = deviation[3] + ((R4[i] - average[3])**2)
        deviation[4] = deviation[4] + ((R5[i] - average[4])**2)
        deviation[5] = deviation[5] + ((R6[i] - average[5])**2)
        deviation[6] = deviation[6] + ((R7[i] - average[6])**2)
        deviation[7] = deviation[7] + ((R8[i] - average[7])**2)

    SD.write(RTT + "\t")
    for i in range(0,columns):
        standard_deviation[i] = round(math.sqrt(deviation[i]/50), 3)
        SD.write(str(standard_deviation[i]) + "\t")
    SD.write("\n")


#---------------------------------------------------------
#                   Main Function
#---------------------------------------------------------
    
round_trip_time = np.empty(59, dtype=int)
for i in range(0,59):
    round_trip_time[i] = 10 + (i*5) - 6
queue_size = {"DropTail", "Threshold100", "Threshold15"}
# queue_size = {"DropTail", "Threshold100"}

for i in queue_size:
    for j in round_trip_time:
        if(i=="Threshold100" or i=="Threshold15"):
            f1 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "msRTTQS.txt"
        else: 
            f1 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "RTTQS.txt"
        f2 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "RTTQD.txt"; 
        calculate_QueueingDelay(f1, str(i), str(j), "QD")
        calculate_latency(f2, str(i), str(j), j, "Lat")
        # print(f1, f2, f3, f4)
for i in queue_size:
    for j in round_trip_time:
        f3 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "RTTLat.txt"; 
        find_mean_SDV(f3, str(i), str(j), "Lat")


