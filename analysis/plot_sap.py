#!/usr/bin/python

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def load_data( filename, index=0 ):
	print("Loading data")
	# Ignore the date column for now
	data = pd.read_csv(filename, memory_map=True,
	nrows=2000 )
#	print("Interpreting dates")
#	dates = pd.to_datetime(data.iloc[:,0], format="%Y-%m-%d %H:%M:%S")
#	return (data.iloc[:,1:],dates)
	data.set_index("Date")
	return data


# Takes a segment of data and calculates sap flow
# Upper and Lower are pandas series, assumed to be the same length
def calculate_flow( upper, lower ):
	k = 1 # Not sure what constant to use
	l = int(min(upper.size, lower.size)/2)
	upper_slope = upper[l:].mean() - upper[:l].mean()
	lower_slope = lower[l:].mean() - upper[:l].mean()
	#print("Upper slope: ", upper_slope, "Lower slope: ", lower_slope)
	flow = k * np.log(upper_slope/lower_slope)
	return (flow, upper_slope, lower_slope)


# The input is a pandas series
# This function returns a list of tuples
# I don't feel like actually finding the peaks and troughs, since our
# our data is pretty noisy right now. So, we're doing this period-based.
def find_peaks( data ):
	# Rise time is about 40-60 seconds long (150 - 200 samples)
	# Before the rise time, we may see the heat pulse of 6 seconds long.
	# Period of the whole cycle is 5 minutes, or 990 samples
	# In our case, we can start at zero
	period = 990 # 5 minute period
	rise_time = 150 # 40 second Rise time
	size = data.shape[0]
	flowrate = np.empty((int(size/period+1), 3))
	j = 0
	for i in range(0,size,period):
		end = i+rise_time
		flowrate[j,:]=calculate_flow(data.iloc[i:end, 0],data.iloc[i:end, 1])
		j = j + 1
	return flowrate
	
def main():
	data = load_data("sapflow.csv", index=22) # The first 22 datapoints are junk

	print("Calculating sap flow")
	flowrate = find_peaks(data)

	print("Plotting data")
	plt.plot(flowrate[:,1:])
	plt.show()


data = load_data("sapflow.csv")
