import numpy as np
from scipy.special import digamma, gamma
from random import shuffle
import math

depths = list()

with open('depths') as md_f:
    lines = md_f.readlines()
    depths = [float(l[:-1]) for l in lines]

print(depths)

r = 128
p = 0.362
learning_rate = .001 
while True:
    shuffle(depths)
    r_start = r
    p_start = p
    for depth in depths:
        x = depth
        grad_r = digamma(x + r) - digamma(r) + np.log(1 - p)
        grad_p = (x / p) - (r / (1 - p))
        r = r + learning_rate * grad_r
        p = p + learning_rate * grad_p
    print(r)
    print(p)
    if (np.abs(r_start - r) < 1e-5 and np.abs(p_start - p) < 1e-5):
        break

print('p: ' + str(p))
print('r: ' + str(r))
