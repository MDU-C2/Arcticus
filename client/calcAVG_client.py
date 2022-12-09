import csv
import math
import numpy as np
import matplotlib.pyplot as plt
MS_CONVERTER = 1000
CPU_MS_CONVERTER = 1000
 
columnNumber = 0
rcvVideoTime = [float(l.split(';')[columnNumber]) for l in open('rcvVideoTime.csv', 'r').readlines()]
rcvVideoCPU = [float(l.split(';')[columnNumber]) for l in open('rcvVideoCPU.csv', 'r').readlines()]
sendCtrlTime = [float(l.split(';')[columnNumber]) for l in open('sendCtrlTime.csv', 'r').readlines()]
sendCtrlCPU = [float(l.split(';')[columnNumber]) for l in open('sendCtrlCPU.csv', 'r').readlines()]
f,axs=plt.subplots(2,2)

rcvVideoTime = rcvVideoTime[30:] # Ugly fix, want to remove first 30 indices
rcvVideoCPU = rcvVideoCPU[30:] # Ugly fix, want to remove first 30 indices
sendCtrlTime = sendCtrlTime[30:] # Ugly fix, want to remove first 30 indices
sendCtrlCPU = sendCtrlCPU[30:] # Ugly fix, want to remove first 30 indices

## For removing some negative values, not sure why they occur but they affect the results very little ##
rcvVideoTime = [ele for ele in rcvVideoTime if ele >0]
rcvVideoCPU = [ele for ele in rcvVideoCPU if ele >0]
sendCtrlTime = [ele for ele in sendCtrlTime if ele >0]
sendCtrlCPU = [ele for ele in sendCtrlCPU if ele >0]



## For recieve video, in uptime ##
mean = (sum(rcvVideoTime) / len(rcvVideoTime)) / MS_CONVERTER
max_value = max(rcvVideoTime) / MS_CONVERTER
min_value = min(rcvVideoTime) /MS_CONVERTER
std_value = (max_value - min_value)/ MS_CONVERTER
std_dev = np.std(rcvVideoTime, dtype= np.float64)

print("For receiving video, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvVideoTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[0,0].plot(rcvVideoTime, "*")
axs[0,0].set_xlabel('Samples')
axs[0,0].set_ylabel('ms')
axs[0,0].set_title("Timestamps for receiving video in time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For recieve video, in CPU time ##
mean = (sum(rcvVideoCPU) / len(rcvVideoCPU)) * CPU_MS_CONVERTER
max_value = max(rcvVideoCPU) * CPU_MS_CONVERTER
min_value = min(rcvVideoCPU) * CPU_MS_CONVERTER
std_dev = np.std(rcvVideoCPU, dtype= np.float64) * CPU_MS_CONVERTER

print("For receiving video, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvVideoCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[1,0].plot(rcvVideoCPU, "*")
axs[1,0].set_xlabel('Samples')
axs[1,0].set_ylabel('ms')
axs[1,0].set_title("Timestamps for receiving video in CPU-time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For send control message, in uptime ##
mean = (sum(sendCtrlTime) / len(sendCtrlTime))/MS_CONVERTER
max_value = max(sendCtrlTime)/MS_CONVERTER
min_value = min(sendCtrlTime)/MS_CONVERTER
std_dev = np.std(sendCtrlTime, dtype= np.float64)/MS_CONVERTER

print("For sending control messages, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendCtrlTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[0,1].plot(sendCtrlTime, "*")
axs[0,1].set_xlabel('Samples')
axs[0,1].set_ylabel('ms')
axs[0,1].set_title("Timestamps for sending control messages in time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For send control message, in CPU ##
mean = (sum(sendCtrlCPU) / len(sendCtrlCPU)) * CPU_MS_CONVERTER
max_value = max(sendCtrlCPU) * CPU_MS_CONVERTER
min_value = min(sendCtrlCPU) * CPU_MS_CONVERTER
std_value = max_value - min_value * CPU_MS_CONVERTER
std_dev = np.std(sendCtrlCPU, dtype= np.float64) * CPU_MS_CONVERTER

print("For sending control messages, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendCtrlCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[1,1].plot(sendCtrlCPU, "*")
axs[1,1].set_xlabel('Samples')
axs[1,1].set_ylabel('ms')
axs[1,1].set_title("Timestamps for sending control messages in CPU-time taken")

plt.show()
