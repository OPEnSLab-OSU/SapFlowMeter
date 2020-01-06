import pandas as pd
import datetime as dt
import matplotlib.pyplot as plt
import numpy as np
import slope
import math

# Test sapflow calculation with linear regression to find slope
def improved_sapflow( dates, data, seconds=50):
  p = dt.timedelta(seconds=seconds)
  sapflow = np.empty(length)
  for i in range(length):
    t = dates[i] #Get the timestamp for this index
    a = data.loc[t-p:t,:] # Get the data corresponding to that date range
    # Calculate the slope using linear regression
    c1 = slope.linreg(a.iloc[:,0])
    c2 = slope.linreg(a.iloc[:,1])
    # Sapflow is the log of the ratio of the slopes
    sapflow[i] = math.log(c1[1]/c2[1])
  #plt.plot(dates,sapflow)
  return sapflow

# Load the naive sapflow data
condensed = pd.read_csv('../data/sapflow(02).csv', index_col=0,
  parse_dates = [0], infer_datetime_format = True, usecols=[0, 6] )
dates = condensed.index
length = condensed.shape[0]
fname = '../data/temperature_log.csv'
print("Read condensed data")


# Load the whole measurement log
data = pd.read_csv(fname , index_col=0, #nrows = 1000,
  parse_dates =[0], infer_datetime_format = True)

print("Loaded the entire temperature log")

flow = improved_sapflow(dates, data)
print("Calculated sapflow")
condensed['sapflow']=flow
condensed.to_csv("good.csv")
print("Saved data")

# Fixme: Make prettier plots
#plt.show()
