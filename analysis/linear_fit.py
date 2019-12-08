import pandas as pd
import datetime as dt
import matplotlib.pyplot as plt
import numpy as np
import slope
import math

condensed = pd.read_csv('../data/sapflow(06).csv', index_col=0,
  parse_dates = [0], infer_datetime_format = True, usecols=[0, 5] )
condensed.plot()
dates = condensed.index
length = condensed.shape[0]
#fname = 'sapflow(06).csv'
fname = '../data/temperature_log.csv'

data = pd.read_csv(fname , index_col=0, #nrows = 1000,
  parse_dates =[0], infer_datetime_format = True)

p = dt.timedelta(minutes=1)
sapflow = np.empty(length)
for i in range(length):
  t = dates[i]
  a = data.loc[t-p:t,:]
  c1 = slope.linreg(a.iloc[:,0])
  c2 = slope.linreg(a.iloc[:,1])
  try:
    sapflow[i] = math.log(c1[1]/c2[1])
  except:
    sapflow[i] = -1

plt.plot(dates,sapflow)
plt.show()
