import pandas as pd
import datetime as dt
import matplotlib.pyplot as plt
import numpy as np
import slope

#fname = 'sapflow(06).csv'
fname = '../data/temperature_log.csv'

data = pd.read_csv(fname , index_col=0, #nrows = 1000,
  parse_dates =[0], infer_datetime_format = True)

p = dt.timedelta(minutes=5)
l = dt.timedelta(minutes=1)
h = data.index[-1] - l
# Print segments of data with the linear regression line
while 1:
  a = data.loc[h:h+l,:]
  a.plot()
  size = a.shape
  x = np.arange(size[0])
  c = slope.linreg(a.iloc[:,0])
  y = x*c[1]+c[0]
  plt.plot(a.index, y)
  c = slope.linreg(a.iloc[:,1])
  y = x*c[1]+c[0]
  plt.plot(a.index, y)
  plt.show()
  h -= p
