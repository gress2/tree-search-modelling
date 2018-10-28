from anytree import AnyNode, RenderTree
from anytree.exporter import DotExporter

def mix(group):
    n = len(group)
    mu_p = sum([(1/n) * elem[0] for elem in group])
    var_t1 = sum([(1/n) * elem[1] for elem in group])
    var_t2 = sum([(1/n) * elem[0]**2 for elem in group])
    var_t3 = mu_p**2
    var_p = var_t1 + var_t2 - var_t3
    return (mu_p, var_p)

arity = 3
l = [(10, 2), (20, 5), (5, 1), (33, 6.5), (18, 9.6), (0, 12), (20, 15), (50, 8), (5, 2), (8, 9), (100, 7), (50, 8)]
groups = list()
tiers = list()

while len(l) > 1:
    print(l)
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
    nodes.append([AnyNode(mu=elem[0], sigma2=elem[1]) for elem in tier])

id_ctr = 0

def assign_name(node):
    global id_ctr
    node.name = 'mu:' + '{:10.2f}'.format(node.mu) + '\n sigma2:' + '{:10.2f}'.format(node.sigma2) + '\n id: ' + str(id_ctr)
    id_ctr += 1

root = nodes[0][0]
assign_name(root)

for i in range(len(tiers)):
    if i > 0:
        # needs parent assignments
        for j in range(len(tiers[i])):
            parent_idx = int(j/arity)
            nodes[i][j].parent = nodes[i-1][parent_idx] 
            assign_name(nodes[i][j])

DotExporter(root).to_picture('tree.png')

