import numpy as np
from scipy.stats import dirichlet

for x in np.linspace(.1, 3, 20):
    print(x, dirichlet.pdf(np.array([.9, .05, .05]), np.ones(3) / x))


