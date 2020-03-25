import numpy as np

# Implements linear regression to find the slope of a vector.
# Formula is X'*y / X'*X, where each row of X is [1,i]
# We implement an optimized version here, which we will
# translate to C++ for embedded use
def linreg(y):
  n = y.shape[0]
  # Build [1 x]'*[1 x]
  a = np.empty((2,2))
  a[0,0] = n
  a[0,1] = n*(n-1)*.5 #sum of 1:n
  a[1,0] = a[0,1]
  a[1,1] = a[0,1]*(n-0.5)*(2./3.) #sum of 1*1:n*n
  # Invert the matrix
  det = a[0,0]*a[1,1] - a[0,1]*a[0,1]
  tmp = a[0,0]
  a[0,0] = a[1,1] / det
  a[1,1] = tmp / det
  a[0,1] = a[0,1] / -det
  a[1,0] = a[0,1] # We know these are the same
  # Build [1 x]'*y
  b = np.zeros(2)
  b[0] = y.sum()
  x = np.arange(n)
  b[1] = x.dot(y)
  # Calculate slope and offset
  offset = a[0,0]*b[0] + a[1,0]*b[1]
  slope = a[0,1]*b[0] + a[1,1]*b[1]
  return( [offset, slope] )

# Test the function
if 0:
  x = np.arange(20.)
  s1 = [10.3, 2.05]
  y1 = s1[1] * x + s1[0]
  noise =1 * np.random.random(y1.shape[0])
  y1 += noise
  s2 = linreg(y1)
  print(s1, s2)
  y2 = x*s2[1] + s2[0]

  import matplotlib.pyplot as plt
  plt.plot(x, y1)
  plt.plot(x, y2)
  plt.show()

