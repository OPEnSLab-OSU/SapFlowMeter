#!/usr/bin/python

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

period = 1000

def load_data( filename, index ):
	print("Loading data")
	headers = ['Date', 'Upper', 'Lower']
	dtypes = {'Date':'str', 'Upper':'float', 'Lower':'float'}
	parse_dates = ['Date']
	data = pd.read_csv(filename, memory_map=True, usecols = (1,2), #skiprows=index, nrows=period*200, 
	header=None, names=headers) #, dtype=dtypes, parse_dates=parse_dates)
	return data

def filter_outliers( data ):
	epsilon = 0.2
	for i in range(1,data.size):
		if abs(data.iloc[i] - data.iloc[i-1]) > epsilon:
			data.iloc[i]=data.iloc[i-1]

def calculate_flow( data ):
	columns = ('Upper','Lower')
	frames = int(data.shape[0]/period)
	pulse = np.empty((frames,2))
	for i in range(frames):
		p = data.loc[:,columns].iloc[i*period:(i+1)*period].std()
		pulse[i,:]=p
	return pulse
data = load_data(filename="sapflow.csv", index=960)

print("Filtering outliers")
filter_outliers( data.loc[:,'Upper'] )
filter_outliers( data.loc[:,'Lower'] )

print("Calculating flow")
pulse = calculate_flow( data )

print("Plotting data")
#plt.plot(data['Upper','Lower'])
plt.plot(pulse)
print("Showing plot")
plt.show()
