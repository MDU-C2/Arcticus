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

#rcvVideoTime = rcvVideoTime/MS_CONVERTER
for i in range(len(rcvVideoTime)):
    rcvVideoTime[i] = rcvVideoTime[i]*(1/MS_CONVERTER)
#sendCtrlTime = sendCtrlTime/MS_CONVERTER
for i in range(len(sendCtrlTime)):
    sendCtrlTime[i] = sendCtrlTime[i]*(1/MS_CONVERTER)
#rcvVideoCPU = CPU_MS_CONVERTER*rcvVideoCPU
for i in range(len(rcvVideoCPU)):
    rcvVideoCPU[i] = rcvVideoCPU[i]*CPU_MS_CONVERTER
#sendCtrlCPU = CPU_MS_CONVERTER*sendCtrlCPU
for i in range(len(sendCtrlCPU)):
    sendCtrlCPU[i] = sendCtrlCPU[i]*CPU_MS_CONVERTER

## For removing some negative values, not sure why they occur but they affect the results very little ##
rcvVideoTime = [ele for ele in rcvVideoTime if ele >0]
rcvVideoCPU = [ele for ele in rcvVideoCPU if ele >0]
sendCtrlTime = [ele for ele in sendCtrlTime if ele >0]
sendCtrlCPU = [ele for ele in sendCtrlCPU if ele >0]



## For recieve video, in uptime ##
mean = (sum(rcvVideoTime) / len(rcvVideoTime)) 
max_value = max(rcvVideoTime) 
min_value = min(rcvVideoTime) 
std_dev = np.std(rcvVideoTime, dtype= np.float64)

print("For receiving video, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvVideoTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[0,0].plot(rcvVideoTime, ".")
axs[0,0].set_xlabel('Samples')
axs[0,0].set_ylabel('ms')
axs[0,0].set_title("Timestamps for receiving video in time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For recieve video, in CPU time ##
mean = (sum(rcvVideoCPU) / len(rcvVideoCPU)) 
max_value = max(rcvVideoCPU) 
min_value = min(rcvVideoCPU) 
std_dev = np.std(rcvVideoCPU, dtype= np.float64) 

print("For receiving video, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvVideoCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[1,0].plot(rcvVideoCPU, ".")
axs[1,0].set_xlabel('Samples')
axs[1,0].set_ylabel('ms')
axs[1,0].set_title("Timestamps for receiving video in CPU-time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For send control message, in uptime ##
mean = (sum(sendCtrlTime) / len(sendCtrlTime))
max_value = max(sendCtrlTime)
min_value = min(sendCtrlTime)
std_dev = np.std(sendCtrlTime, dtype= np.float64)

print("For sending control messages, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendCtrlTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[0,1].plot(sendCtrlTime, ".")
axs[0,1].set_xlabel('Samples')
axs[0,1].set_ylabel('ms')
axs[0,1].set_title("Timestamps for sending control messages in time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For send control message, in CPU ##
mean = (sum(sendCtrlCPU) / len(sendCtrlCPU)) 
max_value = max(sendCtrlCPU)
min_value = min(sendCtrlCPU) 
std_dev = np.std(sendCtrlCPU, dtype= np.float64)

print("For sending control messages, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendCtrlCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[1,1].plot(sendCtrlCPU, ".")
axs[1,1].set_xlabel('Samples')
axs[1,1].set_ylabel('ms')
axs[1,1].set_title("Timestamps for sending control messages in CPU-time taken")

plt.show()
