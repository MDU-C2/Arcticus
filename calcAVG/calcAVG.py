import csv
import math
import numpy as np
import matplotlib.pyplot as plt

columnNumber = 0
data = [float(l.split(';')[columnNumber]) for l in open('Encode_timestamp.csv', 'r').readlines()]
data2 = [float(l.split(';')[columnNumber]) for l in open('rcvVideo_timestamp.csv', 'r').readlines()]


mean = sum(data) / len(data)
max_value = max(data)
min_value = min(data)
std_value = max_value - min_value
std_dev = np.std(data, dtype= np.float64)


print(f'Average value: {mean}' )
print(f'Indeces amount: {len(data)}' )
print(f'Max value: {max_value}' )
print(f'Min value: {min_value}' )
print(f'Standard deviation: {std_dev}' )

plt.plot(data2, "*")
plt.show()

