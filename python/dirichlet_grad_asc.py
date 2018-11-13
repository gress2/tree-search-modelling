import ast
import numpy as np
import pymc3 as pm
from theano import tensor as tt
from scipy.special import digamma, gamma
from random import shuffle
import math

def normalize(l):
    sum_l = sum(l)
    for i in range(len(l)):
        l[i] /= sum_l
    return l

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

for i in range(len(children_means)):
    children_means[i] = normalize(children_means[i])
    children_vars[i] = normalize(children_vars[i])

''' 
mu_c = delta * n * mu_p
delta ~ Dir(1 / gamma)
var_c = beta * n * (var_p - Var[mu_c])
beta ~ Dir(1 / gamma)
gamma ~ U(10)
'''

def dirichlet_ll(X, alpha):
    k = len(X)
    t1 = np.log(gamma(k * alpha))
    t2 = k * np.log(gamma(alpha))
    t3 = (alpha - 1) * np.sum(np.log(X)) 
    ll = t1 - t2 + t3
    return ll


def dirichlet_total_ll(Xs, alpha):
    total_ll = 0
    for X in Xs:
        total_ll += dirichlet_ll(X, alpha)
    return total_ll

learning_rate = 1e-2
alpha = 1
iters = 100000
cnt = 0

while(cnt < iters):
    shuffle(children_vars)
    alpha_start = alpha
    # sgd 
    for vars in children_vars:
        x = np.array(vars)
        k = len(x)
        gradient = k*digamma(alpha*k) - (k*digamma(alpha)) + np.sum(np.log(x))
        alpha = alpha + learning_rate * gradient
    cnt += 1
    if (np.abs(alpha_start - alpha) < .0001):
        break
print(alpha)

