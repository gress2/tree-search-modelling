import ast
import numpy as np
from random import shuffle

with open('same_game_nc_dist') as dist_f:
    z = [(int(l.split(' ')[0]), int(l.split(' ')[1])) for l in dist_f.readlines()] 

ncs, depths = zip(*z)
print(ncs, depths)
        
t1 = 1 
t2 = 1
t3 = 1
learning_rate = 1e-8

while True:
    concat = list(zip(ncs, depths))
    shuffle(concat)
    ncs, depths = zip(*concat)
    t1_start = t1
    t2_start = t2
    t3_start = t3
    for i in range(len(ncs)):
        x = ncs[i]
        d = depths[i]
        grad_t1 = (x / (t1 + t2 * d + t2 * d**2)) - 1
        grad_t2 = (d * x / (t1 + t2 * d + t2 * d**2)) - d
        grad_t3 = (d**2 * x / (t1 + t2 * d + t2 * d**2)) - d**2
        t1 = t1 + learning_rate * grad_t1
        t2 = t2 + learning_rate * grad_t2
        t3 = t3 + learning_rate * grad_t3
    if (np.abs(t1_start - t1) < 1e-3 and np.abs(t2_start - t2) < 1e-3 and np.abs(t3_start - t3 < 1e-3)):
        break
    print('t1: ' + str(t1))
    print('t2: ' + str(t2))
    print('t3: ' + str(t3))
print('t1: ' + str(t1))
print('t2: ' + str(t2))
print('t3: ' + str(t3))


