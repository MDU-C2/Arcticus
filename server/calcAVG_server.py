import csv
import math
import numpy as np
import matplotlib.pyplot as plt
 
columnNumber = 0
sendVideoTime = [float(l.split(';')[columnNumber]) for l in open('sendVideoTime.csv', 'r').readlines()]
sendVideoCPU = [float(l.split(';')[columnNumber]) for l in open('sendVideoCPU.csv', 'r').readlines()]
rcvCtrlTime = [float(l.split(';')[columnNumber]) for l in open('rcvCtrlTime.csv', 'r').readlines()]
rcvCtrlCPU = [float(l.split(';')[columnNumber]) for l in open('rcvCtrlCPU.csv', 'r').readlines()]

f,axs=plt.subplots(2,2)
## For sending video, in uptime ##
mean = sum(sendVideoTime) / len(sendVideoTime)
max_value = max(sendVideoTime)
min_value = min(sendVideoTime)
std_value = max_value - min_value
std_dev = np.std(sendVideoTime, dtype= np.float64)

print("For sending video, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendVideoTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[0,0].plot(sendVideoTime, "*")
axs[0,0].set_xlabel('Samples')
axs[0,0].set_ylabel('ms')
axs[0,0].set_title("Timestamps for sending video in time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For sending video, in CPU time ##
mean = sum(sendVideoCPU) / len(sendVideoCPU)
max_value = max(sendVideoCPU)
min_value = min(sendVideoCPU)
std_value = max_value - min_value
std_dev = np.std(sendVideoCPU, dtype= np.float64)

print("For sending video, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(sendVideoCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[1,0].plot(sendVideoCPU, "*")
axs[1,0].set_xlabel('Samples')
axs[1,0].set_ylabel('ms')
axs[1,0].set_title("Timestamps for sending video in CPU-time taken")


## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For receiving control message, in uptime ##
mean = sum(rcvCtrlTime) / len(rcvCtrlTime)
max_value = max(rcvCtrlTime)
min_value = min(rcvCtrlTime)
std_value = max_value - min_value
std_dev = np.std(rcvCtrlTime, dtype= np.float64)

print("For receiving control messages, in time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvCtrlTime)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[0,1].plot(rcvCtrlTime, "*")
axs[0,1].set_xlabel('Samples')
axs[0,1].set_ylabel('ms')
axs[0,1].set_title("Timestamps for receiving control messages in time taken")

## Some empty lines to make prints clearer ##
print("------------------------------")
print("------------------------------")

## For receiving control message, in CPU ##
mean = sum(rcvCtrlCPU) / len(rcvCtrlCPU)
max_value = max(rcvCtrlCPU)
min_value = min(rcvCtrlCPU)
std_value = max_value - min_value
std_dev = np.std(rcvCtrlCPU, dtype= np.float64)

print("Control messages, in CPU-time taken")
print(f'Average value: {mean}' )
print(f'Indeces amount: {len(rcvCtrlCPU)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

axs[1,1].plot(rcvCtrlCPU, "*")
axs[1,1].set_xlabel('Samples')
axs[1,1].set_ylabel('ms')
axs[1,1].set_title("Timestamps for receving control messages in CPU-time taken")

plt.show()
