import codecs
from collections import deque

import numpy as np
import matplotlib.pyplot as plt
import math
import argparse

parser = argparse.ArgumentParser(description='Take path to plot file')
parser.add_argument('--path', type=str, help='Path to plot file')
args = parser.parse_args()


if _name_ == '_main_':
    arr = np.zeros((2, 3))
    print(arr)
    with codecs.open("tcp-example.tr", 'r') as file:
        file = file.readlines()

    newFile = []
    for i in range(len(file)):
        x = file[i].split("/")
        if(x[0][0] == 'r'):
            continue
        else:
            newFile.append(file[i])

    packets = {'0': deque(), '1': deque(), '2': deque()}
    delay = []
    endTimes = []
    keys, data = {'0': [], '1': [], '2': []}, {'0': [], '1': [], '2': []}
    for i in range(len(newFile)):
        temp = newFile[i]
        x = temp.split(" ")
        if(x[0] == '+'):
            packets[x[2][10]].append(float(x[1]))
        elif(x[0] == '-'):
            startTime = packets[x[2][10]].popleft()
            time = float(x[1]) - startTime
            delay.append(time)
            endTimes.append(float(x[1]))
            data[x[2][10]].append(time)
            keys[x[2][10]].append(float(x[1]))
            # plot[float(x[1])] = time[0]


    # print(newFile[0])

    # plt.plot(keys['0'], data['0'])
    # plt.show()

    plt.figure(figsize=(10,6))
    plt.plot(keys['1'], data['1'])
    # plt.plot(plot.keys(), plot.values())
    plt.xlabel("Time")
    plt.ylabel("Queueing Delay")
    plt.title("Queueing Delay Plot")
    plt.savefig("time_plot1.png")