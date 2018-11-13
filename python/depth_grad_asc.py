import numpy as np
from scipy.special import digamma, gamma
from random import shuffle

depths = list()

with open('mixing_data') as md_f:
    lines = md_f.readlines()
    for line in lines:
        split = line.split(' ')
        depth = float(split[2])
        depths.append(depth)

r = 1
p = 0.5
learning_rate = 1e-3
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
    if (np.abs(r_start - r) < 1e-3 and np.abs(p_start - p) < 1e-3):
        break

print('p: ' + str(p))
print('r: ' + str(r))
