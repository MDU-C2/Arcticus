import csv
import math
import numpy as np
MS_CONVERTER = 1000
CPU_MS_CONVERTER = 1000
 
columnNumber = 0
sendVideoTime = [float(l.split(';')[columnNumber]) for l in open('sendVideoTime.csv', 'r').readlines()]
sendVideoCPU = [float(l.split(';')[columnNumber]) for l in open('sendVideoCPU.csv', 'r').readlines()]
rcvCtrlTime = [float(l.split(';')[columnNumber]) for l in open('rcvCtrlTime.csv', 'r').readlines()]
rcvCtrlCPU = [float(l.split(';')[columnNumber]) for l in open('rcvCtrlCPU.csv', 'r').readlines()]


sendVideoTime = sendVideoTime[30:] # Ugly fix, want to remove first 30 indices
sendVideoCPU = sendVideoCPU[30:] # Ugly fix, want to remove first 30 indices
rcvCtrlTime = rcvCtrlTime[30:] # Ugly fix, want to remove first 30 indices
rcvCtrlCPU = rcvCtrlCPU[30:] # Ugly fix, want to remove first 30 indices

#rcvVideoTime = rcvVideoTime/MS_CONVERTER
for i in range(len(sendVideoTime)):
    sendVideoTime[i] = sendVideoTime[i]*(1/MS_CONVERTER)
#sendCtrlTime = sendCtrlTime/MS_CONVERTER
for i in range(len(rcvCtrlTime)):
    rcvCtrlTime[i] = rcvCtrlTime[i]*(1/MS_CONVERTER)
#rcvVideoCPU = CPU_MS_CONVERTER*rcvVideoCPU
for i in range(len(sendVideoCPU)):
    sendVideoCPU[i] = sendVideoCPU[i]*CPU_MS_CONVERTER
#sendCtrlCPU = CPU_MS_CONVERTER*sendCtrlCPU
for i in range(len(rcvCtrlCPU)):
    rcvCtrlCPU[i] = rcvCtrlCPU[i]*CPU_MS_CONVERTER


## For removing some negative values, not sure why they occur but they affect the results very little ##
sendVideoTime = [ele for ele in sendVideoTime if ele >0]
sendVideoCPU = [ele for ele in sendVideoCPU if ele >0]
rcvCtrlTime = [ele for ele in rcvCtrlTime if ele >0]
rcvCtrlCPU = [ele for ele in rcvCtrlCPU if ele >0]


## For sending video, in uptime ##
mean = (sum(sendVideoTime) / len(sendVideoTime))
max_value = max(sendVideoTime)
min_value = min(sendVideoTime)
std_dev = np.std(sendVideoTime, dtype= np.float64)

print("For sending video, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendVideoTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For sending video, in CPU time ##
mean = (sum(sendVideoCPU) / len(sendVideoCPU))
max_value = max(sendVideoCPU)
min_value = min(sendVideoCPU)
std_dev = np.std(sendVideoCPU, dtype= np.float64)

print("For sending video, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendVideoCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )



## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For receiving control message, in uptime ##
mean = (sum(rcvCtrlTime) / len(rcvCtrlTime))
max_value = max(rcvCtrlTime)
min_value = min(rcvCtrlTime)
std_dev = np.std(rcvCtrlTime, dtype= np.float64)

print("For receiving control messages, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvCtrlTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For receiving control message, in CPU ##
mean = (sum(rcvCtrlCPU) / len(rcvCtrlCPU))
max_value = max(rcvCtrlCPU)
min_value = min(rcvCtrlCPU)
std_dev = np.std(rcvCtrlCPU, dtype= np.float64)

print("Control messages, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvCtrlCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )


