import matplotlib.pyplot as plt

with open('dist') as dist_f:
    lines = dist_f.readlines()
    means = [float(l) for l in lines]

plt.hist(means, 100)
plt.show()
