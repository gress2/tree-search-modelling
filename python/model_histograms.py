import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

generic_game_name = "generic_game"
same_game_name = "same_game"

fig, axs = plt.subplots(ncols=2, nrows=3)
fig.tight_layout()

axs[0][0].set_title('Generic Game')
axs[0][1].set_title('Same Game')

with open(generic_game_name + '_reward_dist') as dist_f:
    X = [float(l) for l in dist_f.readlines()]
    sns.distplot(X, axlabel='reward', ax=axs[0][0])

with open(same_game_name + '_reward_dist') as dist_f:
    X = [float(l) for l in dist_f.readlines()]
    sns.distplot(X, axlabel='reward', ax=axs[0][1])

with open(generic_game_name + '_nc_dist') as dist_f:
    X = [float(l) for l in dist_f.readlines()]
    sns.distplot(X, axlabel='number of children', ax=axs[1][0])

with open(same_game_name + '_nc_dist') as dist_f:
    X = [float(l) for l in dist_f.readlines()]
    sns.distplot(X, axlabel='number of children', ax=axs[1][1])

with open(generic_game_name + '_td_dist') as dist_f:
    X = [float(l) for l in dist_f.readlines()]
    sns.distplot(X, axlabel='terminal depths', ax=axs[2][0])

with open(same_game_name + '_td_dist') as dist_f:
    X = [float(l) for l in dist_f.readlines()]
    sns.distplot(X, axlabel='terminal depths', ax=axs[2][1])

plt.show()
