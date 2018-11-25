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

def mix(group):
    n = len(group)
    mu_p = sum([(1/n) * elem[0] for elem in group])
    var_t1 = sum([(1/n) * elem[1] for elem in group])
    var_t2 = sum([(1/n) * elem[0]**2 for elem in group])
    var_t3 = mu_p**2
    var_p = var_t1 + var_t2 - var_t3
    return (mu_p, var_p)

arity = 2
l = [(5, 10), (60, 10)]
groups = list()
tiers = list()

while len(l) > 1:
    tiers.append([e for e in l])
    # combine to groups
    while len(l) > 0:
        ct = 0
        group = list()
        while ct < arity and len(l) > 0:
            group.append(l.pop())
            ct += 1
        groups.append(group)
    # flatten groups by creating mixtures
    while len(groups) > 0:
        l.append(mix(groups.pop()))

tiers.append([e for e in l])
tiers.reverse()

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
    x = np.arange(root.mu - 10, root.mu + 10, .1)
    plt.figure()
    plt.title('mu={:10.2f}'.format(root.mu) + ' sigma2={:10.4f}'.format(root.sigma2))
    plt.plot(x, pdf(x))
    plt.savefig('dist' + str(root.id) + '.png')
    graph_str += str(root.id) + ' [image="dist' + str(root.id) + '.png" label=""];\n'
    for child in root.children:
        graph_str += str(root.id) + ' -> ' + str(child.id) + ';\n'
        dfs(child)

dfs(root)

with open('bottom_up_tree.dot', 'w') as dot_f:
    dot_f.write('digraph {\n')
    dot_f.write(graph_str)
    dot_f.write('}\n')



