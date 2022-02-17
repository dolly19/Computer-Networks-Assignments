import pandas as pd
from matplotlib import pyplot as plt

data = pd.read_csv('data.csv')
time = data['Time'].tolist()
length = data['Length'].tolist()
N = len(time)

time_temp = [time[0]]
length_temp =[length[0]]

throughput=[]
j = 0
k = -1

for i in range(1,N) :
	if (time[i] - time[j] < 2):
		length_temp[-1] += length[i]
		k = i
	else:
		tput = length_temp[-1]/(time[i-1]-time_temp[-1])
		throughput.append(tput)
		time_temp.append(time[i])
		length_temp.append(length[i])
		j=i

tput = length_temp[-1]/(time[k]-time_temp[-1])
throughput.append(tput)

plt.plot(time_temp, throughput)
plt.ylabel("Throughputs")
plt.xlabel("Time")
plt.show()


