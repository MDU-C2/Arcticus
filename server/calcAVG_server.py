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



## For removing some negative values, not sure why they occur but they affect the results very little ##
sendVideoTime = [ele for ele in sendVideoTime if ele >0]
sendVideoCPU = [ele for ele in sendVideoCPU if ele >0]
rcvCtrlTime = [ele for ele in rcvCtrlTime if ele >0]
rcvCtrlCPU = [ele for ele in rcvCtrlCPU if ele >0]


## For sending video, in uptime ##
mean = (sum(sendVideoTime) / len(sendVideoTime))/MS_CONVERTER
max_value = max(sendVideoTime)/MS_CONVERTER
min_value = min(sendVideoTime)/MS_CONVERTER
std_dev = np.std(sendVideoTime, dtype= np.float64)/MS_CONVERTER

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
mean = (sum(sendVideoCPU) / len(sendVideoCPU))*CPU_MS_CONVERTER
max_value = max(sendVideoCPU)*CPU_MS_CONVERTER
min_value = min(sendVideoCPU)*CPU_MS_CONVERTER
std_dev = np.std(sendVideoCPU, dtype= np.float64)*CPU_MS_CONVERTER

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
mean = (sum(rcvCtrlTime) / len(rcvCtrlTime))/MS_CONVERTER
max_value = max(rcvCtrlTime)/MS_CONVERTER
min_value = min(rcvCtrlTime)/MS_CONVERTER
std_dev = np.std(rcvCtrlTime, dtype= np.float64)/MS_CONVERTER

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
mean = (sum(rcvCtrlCPU) / len(rcvCtrlCPU))*CPU_MS_CONVERTER
max_value = max(rcvCtrlCPU)*CPU_MS_CONVERTER
min_value = min(rcvCtrlCPU)*CPU_MS_CONVERTER
std_dev = np.std(rcvCtrlCPU, dtype= np.float64)*CPU_MS_CONVERTER

print("Control messages, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvCtrlCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )


