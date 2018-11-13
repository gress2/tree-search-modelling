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

linear_model = True

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
    depths.append(int(depth))
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

learning_rate = 1e-3

if linear_model:
    # alpha = delta + beta * depth
    delta_m = 1
    beta_m = 1
    depths_cpy = [d for d in depths]
    while (True):
        concat = list(zip(children_means, depths_cpy))
        shuffle(concat)
        children_means, depths_cpy = zip(*concat)
        delta_m_start = delta_m
        beta_m_start = beta_m
        for i in range(len(children_means)):
            X = np.array(children_means[i])
            k = len(X)
            depth = depths_cpy[i]
            dg1 = digamma(k*delta_m+k*beta_m*depth)
            dg2 = digamma(delta_m+beta_m*depth)
            sum_log = np.sum(np.log(X))
            grad_delta_m = k*dg1 - k*dg2 + sum_log
            grad_beta_m = depth*k*dg1 - depth*k*dg2 + depth * sum_log
            delta_m = delta_m + learning_rate * grad_delta_m
            beta_m = beta_m + learning_rate * grad_beta_m
        if (np.abs(delta_m_start - delta_m) < 1e-3 and np.abs(beta_m_start - beta_m) < 1e-3):
            break
    print('delta_m: ' + str(delta_m))
    print('beta_m: ' + str(beta_m))
    delta_v = 1
    beta_v = 1
    depths_cpy = [d for d in depths]
    while (True):
        concat = list(zip(children_vars, depths_cpy))
        shuffle(concat)
        children_vars, depths_cpy = zip(*concat)
        delta_v_start = delta_v
        beta_v_start = beta_v
        for i in range(len(children_vars)): 
            X = np.array(children_vars[i])
            k = len(X)
            depth = depths_cpy[i]
            dg1 = digamma(k*delta_v+k*beta_v*depth)
            dg2 = digamma(delta_v+beta_v*depth)
            sum_log = np.sum(np.log(X))
            grad_delta_v = k*dg1 - k*dg2 + sum_log
            grad_beta_v = depth*k*dg1 - depth*k*dg2 + depth * sum_log
            delta_v = delta_v + learning_rate * grad_delta_v
            beta_v = beta_v + learning_rate * grad_beta_v
        if (np.abs(delta_v_start - delta_v) < 1e-3 and np.abs(beta_v_start - beta_v) < 1e-3):
            break
    print('delta_v: ' + str(delta_v))
    print('beta_v: ' + str(beta_v))

else:
    alpha_m = 1
    while (True):
        shuffle(children_means)
        alpha_m_start = alpha_m
        for means in children_means:
            X = np.array(means)
            k = len(X)
            grad_m = k*digamma(alpha_m * k) - (k*digamma(alpha_m)) + np.sum(np.log(X)) 
            alpha_m = alpha_m + learning_rate * grad_m 
        if (np.abs(alpha_m_start - alpha_m) < 1e-3):
            break
    alpha_v = 1
    while (True):
        shuffle(children_vars)
        alpha_v_start = alpha_v
        for vars in children_vars:
            X = np.array(vars)
            k = len(X)
            grad_v = k*digamma(alpha_v * k) - (k*digamma(alpha_v)) + np.sum(np.log(X)) 
            alpha_v = alpha_v + learning_rate * grad_v 
        if (np.abs(alpha_v_start - alpha_v) < 1e-3):
            break
    print('alpha_m: ' + str(alpha_m))
    print('alpha_v: ' + str(alpha_v))

