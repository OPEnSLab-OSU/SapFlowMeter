import numpy as np

#Implements linear regression to find the slope of a vector.
def linreg(y):
  l = y.shape[0]
  x = np.arange(l)
  a = np.ones((2,2))
  a[0,0] = l
  a[0,1] = x.sum()
  a[1,0] = a[0,1]
  a[1,1] = x.dot(x)
  a = np.linalg.inv(a)
  b = np.zeros(2)
  b[0] = y.sum()
  b[1] = x.dot(y)
  c = np.matmul(a,b)
  # c is [offset, slope]
  return( c )

# Test the function
if 0:
  x = np.arange(20.)
  s1 = [10, 2]
  y1 = s1[1] * x + s1[0]
  noise =5 * np.random.random(y1.shape[0])
  y1 += noise
  s2 = linreg(y1)
  print(s1, s2)
  y2 = x*s2[1] + s2[0]

  import matplotlib.pyplot as plt
  plt.plot(x, y1)
  plt.plot(x, y2)
  plt.show()

