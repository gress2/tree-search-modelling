import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def fn(x, a, b):
    return a + b*x 

with open('mixing_data') as md_f:
    lines = [l[:-1] for l in md_f.readlines()]
    X = [float(l.split(' ')[2]) for l in lines]
    Y = [float(l.split(' ')[1]) for l in lines]

zipped = list(zip(X, Y))
zipped.sort()

X, Y = zip(*zipped)

X = np.array(X)
Y = np.array(Y)

plt.scatter(X, Y, s=1)

popt, pcov = curve_fit(fn, X, Y)

plt.plot(X, fn(X, *popt), 'r-', label='a=%5.3f, b=%5.3f' % tuple(popt))
plt.xlabel('mean')
plt.ylabel('var')
plt.legend()
plt.show()

