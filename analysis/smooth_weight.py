import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import scipy.signal as sig

# Load the naive sapflow data
condensed = pd.read_csv('../data/sapflow(04).csv', index_col=0,
  parse_dates = [0], infer_datetime_format = True, usecols=[0, 6] )

print("Resolving discontinuity")
w = condensed[' weight']
a = np.ediff1d(w)
b = np.nonzero( a > 0.5 )
print(F'Fixing at indexes {b}')
for i in b[0]:
  w.iloc[i+1:] -= a[i]

condensed[' weight'] = w

print("Filtering out quantization noise")
f = sig.cheby2(8, 100, .5, fs=12, output='sos')
a = sig.sosfiltfilt(f, condensed[' weight'], axis=0)
condensed['smoothed weight'] = a

print("Taking Derivative")
b = -1 * 3600. / 7. * sig.savgol_filter(a, 31, 1, 1, axis=0)

print("Printing graph")
from pandas.plotting import register_matplotlib_converters
register_matplotlib_converters()
fig, ax1 = plt.subplots()
ax1.set_xlabel('Date')
ax1.set_ylabel('gravimetric')
ax1.plot(condensed.index, b)
fig.tight_layout()
plt.show()
