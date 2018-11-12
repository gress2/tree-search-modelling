import ast
import numpy as np
import pymc3 as pm
from theano import tensor as tt

with open('mixing_data') as md_f:
    lines = md_f.readlines()

parent_means = list()
parent_vars = list()
depths = list()
children_means = list()
children_vars = list()

num_groups = len(lines)

for line in lines:
    split = line.split(' ')
    parent_mean, parent_var, depth = split[:3]
    c_vals = ast.literal_eval(' '.join(split[3:]))
    c_means = [child[0] for child in c_vals]
    c_vars = [child[1] for child in c_vals]
    parent_means.append(parent_mean)
    parent_vars.append(parent_var)
    depths.append(depth)
    children_means.append(c_means)
    children_vars.append(c_vars)

def stick_breaking(ary):
    portion_remaining = tt.concatenate([[1], tt.extra_ops.cumprod(1 - ary)[:-1]])
    return ary * portion_remaining

''' 
mu_c = delta * n * mu_p
delta ~ Dir(1 / gamma)
var_c = beta * n * (var_p - Var[mu_c])
beta ~ Dir(1 / gamma)
gamma ~ U(10)
'''

model = pm.Model()
with model:
    gamma = pm.Uniform('gamma', 0, 10)
    delta = pm.Deterministic('delta', stick_breaking(np.ones()), shape=num_groups)


