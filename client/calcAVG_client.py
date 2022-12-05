import csv
import math
import numpy as np
import matplotlib.pyplot as plt
 
columnNumber = 0
rcvVideoTime = [float(l.split(';')[columnNumber]) for l in open('rcvVideoTime.csv', 'r').readlines()]
rcvVideoCPU = [float(l.split(';')[columnNumber]) for l in open('rcvVideoCPU.csv', 'r').readlines()]
sendCtrlTime = [float(l.split(';')[columnNumber]) for l in open('sendCtrlTime.csv', 'r').readlines()]
sendCtrlCPU = [float(l.split(';')[columnNumber]) for l in open('sendCtrlCPU.csv', 'r').readlines()]
f,axs=plt.subplots(2,2)


## For recieve video, in uptime ##
mean = sum(rcvVideoTime) / len(rcvVideoTime)
max_value = max(rcvVideoTime)
min_value = min(rcvVideoTime)
std_value = max_value - min_value
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
mean = sum(rcvVideoCPU) / len(rcvVideoCPU)
max_value = max(rcvVideoCPU)
min_value = min(rcvVideoCPU)
std_value = max_value - min_value
std_dev = np.std(rcvVideoCPU, dtype= np.float64)

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
mean = sum(sendCtrlTime) / len(sendCtrlTime)
max_value = max(sendCtrlTime)
min_value = min(sendCtrlTime)
std_value = max_value - min_value
std_dev = np.std(sendCtrlTime, dtype= np.float64)

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
mean = sum(sendCtrlCPU) / len(sendCtrlCPU)
max_value = max(sendCtrlCPU)
min_value = min(sendCtrlCPU)
std_value = max_value - min_value
std_dev = np.std(sendCtrlCPU, dtype= np.float64)

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
