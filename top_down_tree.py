import matplotlib.pyplot as plt
import numpy as np
import scipy.stats

ctr = 0

class node:
    def __init__(self, mu, sigma2):
        global ctr
        self.parent = None
        self.children = list()
        self.mu = mu
        self.sigma2 = sigma2
        self.dist_img = None
        self.id = ctr
        ctr += 1
    def add_child(self, child):
        self.children.append(child)

def get_dispersion_upper_bound(mu, sigma2, n):
    return (sigma2 * n) / ((mu**2 * n) - (mu**2) - 1)

def decompose(mu_p, sigma2_p, dispersion, n):
    mu_c = None
    sigma2_c = None
    it = 0

    dispersion = min(dispersion, get_dispersion_upper_bound(mu_p, sigma2_p, n))

    while True:
        # sample until constraint is met :(
        delta = np.random.dirichlet(np.ones(n) * (1/dispersion))
        mu_c = delta * n * mu_p
        if np.var(mu_c) <= sigma2_p:
            break
        it += 1

    beta = np.random.dirichlet(np.ones(n) * (1/dispersion))
    sigma2_c = beta * n * (sigma2_p - np.var(mu_c))
    return list(zip(mu_c, sigma2_c))

mu = 21.6
sigma2 = 565 

dispersion = 1
arity = 3
num_levels = 3

queue = [(mu, sigma2)]
tiers = list()

level = 0
while level < num_levels - 1: 
    tiers.append([e for e in queue])
    queue_prime = list()
    while len(queue) > 0:
        cur = queue.pop()
        queue_prime += decompose(cur[0], cur[1], dispersion, arity)
    queue = queue_prime
    queue.reverse()
    level += 1

tiers.append([e for e in queue])

nodes = list()
for tier in tiers:
    nodes.append([node(mu=elem[0], sigma2=elem[1]) for elem in tier])

def set_parent(child, parent):
    child.parent = parent
    parent.add_child(child)

root = nodes[0][0]

for i in range(len(tiers)):
    if i > 0:
        # needs parent assignments
        for j in range(len(tiers[i])):
            parent_idx = int(j/arity)
            set_parent(nodes[i][j], nodes[i-1][parent_idx])

graph_str = ''

def dfs(root):
    global graph_str
    # generate graph image using matplotlib
    pdf = lambda x: scipy.stats.norm.pdf(x, root.mu, root.sigma2**.5)
    x = np.arange(root.mu - 100, root.mu + 100)
    plt.figure()
    plt.title('mu={:10.2f}'.format(root.mu) + ' sigma2={:10.4f}'.format(root.sigma2))
    plt.plot(x, pdf(x))
    plt.savefig('dist' + str(root.id) + '.png')
    graph_str += str(root.id) + ' [image="dist' + str(root.id) + '.png" label=""];\n'
    for child in root.children:
        graph_str += str(root.id) + ' -> ' + str(child.id) + ';\n'
        dfs(child)

dfs(root)

with open('top_down_tree.dot', 'w') as dot_f:
    dot_f.write('digraph {\n')
    dot_f.write(graph_str)
    dot_f.write('}\n')

