import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import scipy.signal as sig

# Load the naive sapflow data
condensed = pd.read_csv('../data/sapflow(04).csv', index_col=0,
  parse_dates = [0], infer_datetime_format = True, usecols=[0, 6] )

print("Filtering out quantization noise")
f = sig.cheby2(8, 100, .5, fs=12, output='sos')
a = sig.sosfiltfilt(f, condensed[' weight'], axis=0)
condensed['smoothed weight'] = a

print("Taking Derivative")
b = -1 * sig.savgol_filter(a, 31, 1, 1, axis=0)

print("Printing graph")
from pandas.plotting import register_matplotlib_converters
register_matplotlib_converters()
fig, ax1 = plt.subplots()
ax1.set_xlabel('Date')
ax1.set_ylabel('Weight (kg)')
ax1.plot(condensed)
#ax1.legend('weight', 'smoothed')
print("Second axis")
ax2 = ax1.twinx()
condensed['water used']=b
ax2.set_ylabel('water used')
ax2.plot(condensed.index, b, color=(1,.8,0))
#ax2.legend('water usage')
fig.tight_layout()
plt.show()
