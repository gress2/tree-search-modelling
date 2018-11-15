import ast
import numpy as np
from random import shuffle

ncs = list()
depths = list()

with open('mixing_data') as md_f:
    lines = md_f.readlines()
    for line in lines:
        split = line.split(' ')
        depth = int(split[2])
        depths.append(depth)
        c_vals = ast.literal_eval(' '.join(split[3:]))
        nc = len(c_vals)
        ncs.append(nc)
        
alpha = 1 
beta = 1
learning_rate = 1e-3

while True:
    concat = list(zip(ncs, depths))
    shuffle(concat)
    ncs, depths = zip(*concat)
    alpha_start = alpha
    beta_start = beta
    for i in range(len(ncs)):
        x = ncs[i]
        d = depths[i]
        grad_alpha = (x / (alpha + beta * d)) - 1
        grad_beta = (d * x / (alpha + beta * d)) - d
        alpha = alpha + learning_rate * grad_alpha
        beta = beta + learning_rate * grad_beta
    if (np.abs(alpha_start - alpha) < 1e-3 and np.abs(beta_start - beta) < 1e-3):
        break

print('alpha: ' + str(alpha))
print('beta: ' + str(beta))

