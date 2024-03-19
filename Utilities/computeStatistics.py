import pandas as pd 
import numpy as np
import math

#---------------------------------------------------------
# Calculating Mean and Standard Deviation for Queue Sizes
#---------------------------------------------------------
def find_mean_and_SD_fourValues(f3, QueueType, RTT, Metric):
    df = pd.read_csv(f3, delimiter='\t', header=None)
    filename = "./NewResults/TCPNewReno/Statistics/" + QueueType + "_Mean_" + Metric + ".txt" 
    M = open(filename, "a")
    filename = "./NewResults/TCPNewReno/Statistics/" + QueueType + "_SD_" + Metric + ".txt" 
    SD = open(filename, "a")

    R1 = df[1]
    R2 = df[2]
    R3 = df[3]
    R6 = df[6]
    total = np.zeros(4)
    for i in range(200,250):
        total[0] = total[0] + R1[i]
        total[1] = total[1] + R2[i]
        total[2] = total[2] + R3[i]
        total[3] = total[3] + R6[i]

    M.write(RTT + "\t")
    average = np.zeros(4)
    for i in range(0,4):
        average[i] = total[i] / 50
        M.write(str(average[i]) + "\t")
    M.write("\n")

    deviation = np.zeros(4)
    standard_deviation = np.zeros(4)
    for i in range(200,250):
        deviation[0] = deviation[0] + ((R1[i] - average[0])**2)
        deviation[1] = deviation[1] + ((R2[i] - average[1])**2)
        deviation[2] = deviation[2] + ((R3[i] - average[2])**2)
        deviation[3] = deviation[3] + ((R6[i] - average[3])**2)

    SD.write(RTT + "\t")
    for i in range(0,4):
        standard_deviation[i] = round(math.sqrt(deviation[i]/50), 3)
        SD.write(str(standard_deviation[i]) + "\t")
    SD.write("\n")
#-----------------------------------------------------------------
# Calculating Mean and Standard Deviation for Link Utilization
#-----------------------------------------------------------------
def find_mean_and_SD_sixValues(f2, QueueType, RTT, Metric):
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
    columns = 6
    total = np.zeros(columns)
    for i in range(200,250):
        total[0] = total[0] + R1[i]
        total[1] = total[1] + R2[i]
        total[2] = total[2] + R3[i]
        total[3] = total[3] + R4[i]
        total[4] = total[4] + R5[i]
        total[5] = total[5] + R6[i]

    M.write(RTT + "\t")
    average = np.zeros(columns)
    for i in range(0,columns):
        average[i] = round(total[i] / 50, 3)
        M.write(str(average[i]) + "\t")
    M.write("\n")

    deviation = np.zeros(columns)
    standard_deviation = np.zeros(columns)
    for i in range(200,250):
        deviation[0] = deviation[0] + ((R1[i] - average[0])**2)
        deviation[1] = deviation[1] + ((R2[i] - average[1])**2)
        deviation[2] = deviation[2] + ((R3[i] - average[2])**2)
        deviation[3] = deviation[3] + ((R4[i] - average[3])**2)
        deviation[4] = deviation[4] + ((R5[i] - average[4])**2)
        deviation[5] = deviation[5] + ((R6[i] - average[5])**2)

    SD.write(RTT + "\t")
    for i in range(0,columns):
        standard_deviation[i] = round(math.sqrt(deviation[i]/50), 3)
        SD.write(str(standard_deviation[i]) + "\t")
    SD.write("\n")

#-----------------------------------------------------------------
# Calculating Mean and Standard Deviation for Throughput
#-----------------------------------------------------------------
def find_mean_and_SD_eightValues(f2, QueueType, RTT, Metric):
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
    for i in range(200,250):
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
    for i in range(200,250):
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

for i in queue_size:
    for j in round_trip_time:
        if(i=="Threshold100" or i=="Threshold15"):
            f1 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "msRTTth.txt"
            f2 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "msRTTU.txt"
            f3 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "msRTTQS.txt"
            f4 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "msRTTLoss.txt"
        else: 
            f1 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "RTTth.txt"
            f2 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "RTTU.txt"
            f3 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "RTTQS.txt"
            f4 = "./NewResults/TCPNewReno/" + i + "/" + i + str(j) + "RTTLoss.txt"
        find_mean_and_SD_fourValues(f3, str(i), str(j), "QueueSize")
        find_mean_and_SD_fourValues(f4, str(i), str(j), "Loss")
        find_mean_and_SD_sixValues(f2, str(i), str(j), "U")
        find_mean_and_SD_eightValues(f1, str(i), str(j), "Th")
        # print(f1, f2, f3, f4)


