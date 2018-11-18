#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include <vector>
#include "game.hpp"

static std::default_random_engine generator(time(0));

void find_adjacent(
    const same_game::config_type& cfg,
    same_game::action_type action, 
    std::set<same_game::action_type>& adjacent,
    const same_game::state_type& state
) {

  using action_type = same_game::action_type;
  const int width = cfg.width;
  const int height = cfg.height;

  adjacent.insert(action);
  short x = action.first;
  short y = action.second;
  
  short tile = state[x][y];

  action_type up(x, y + 1);
  action_type right(x + 1, y);
  action_type down(x, y - 1);
  action_type left(x - 1, y);

  if (!adjacent.count(up) && up.second < width && 
      state[up.first][up.second] == tile) {
    find_adjacent(cfg, up, adjacent, state);
  }

  if (!adjacent.count(right) && right.first < width && 
      state[right.first][right.second] == tile) {
    find_adjacent(cfg, right, adjacent, state);
  }

  if (!adjacent.count(down) && down.second >= 0 && 
      state[down.first][down.second] == tile) {
    find_adjacent(cfg, down, adjacent, state);
  }

  if (!adjacent.count(left) && left.first >= 0 && 
      state[left.first][left.second] == tile) {
    find_adjacent(cfg, left, adjacent, state);
  }
}

void collapse(const same_game::config_type& cfg, same_game::state_type& state) {
  const int width = cfg.width;
  for (int x = 0; x < width; x++) {
    std::vector<short>& col = state[x];
    for (auto it = col.begin(); it != col.end();) {
      if (*it == 0) {
        it = col.erase(it);
      } else {
        ++it;
      }
    }
    int num_to_fill = width - col.size();
    col.insert(col.end(), num_to_fill, 0);
  }

  for (auto it = state.begin(); it != state.end();) {
    std::vector<short>& col = *it;
    bool all_zero = std::all_of(col.begin(), col.end(),
        [](short i) { return i == 0; });
    if (all_zero) {
      it = state.erase(it);
    } else {
      ++it;
    }
  }

  int num_to_fill = width - state.size();
  std::vector<short> to_fill(width, 0);
  state.insert(state.end(), num_to_fill, to_fill);
}

bool is_game_over(
  const same_game::config_type& cfg, 
  const same_game::state_type& state
) {
  const int width = cfg.width;
  const int height = cfg.height;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      short tile = state[x][y];
      if (tile == 0) {
        break;
      }
      if (y + 1 < width && state[x][y] == tile) {
        return false;
      }
      if (x + 1 < width && state[x][y] == tile) {
        return false;
      }
    }
  }
  return true;
}

int num_tiles_remaining(
  const same_game::config_type& cfg, 
  const same_game::state_type& state
) {
  const int width = cfg.width;
  const int height = cfg.height;

  int ctr = 0;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (state[x][y] == 0) {
        continue;
      } 
      ctr++;
    }
  }
  return ctr;
}

template <class Game>
float make_move_impl(
  const typename Game::config_type& cfg,
  const typename Game::action_type& action, 
  typename Game::state_type& state
);

template <>
float make_move_impl<same_game>(
  const same_game::config_type& cfg,
  const same_game::action_type& action, 
  same_game::state_type& state
) {

  using action_type = typename same_game::action_type;

  std::set<action_type> adjacent;
  find_adjacent(cfg, action, adjacent, state);

  for (auto& elem : adjacent) {
    state[elem.first][elem.second] = 0;
  }

  collapse(cfg, state);

  int num_removed = adjacent.size();
  int reward = (num_removed - 2) * (num_removed - 2);

  if (is_game_over(cfg, state)) {
    int num_remaining = num_tiles_remaining(cfg, state);
    if (!num_remaining) {
      reward += 1000;
    } else {
      reward -= (num_remaining - 2) * (num_remaining - 2);
    }
  }
  return reward;
}

template <class Game>
std::vector<typename Game::action_type> get_moves_impl(
  const typename Game::config_type& cfg, 
  const typename Game::state_type& state
);

template <>
std::vector<same_game::action_type> get_moves_impl<same_game>(
  const same_game::config_type& cfg,
  const same_game::state_type& state
) {
  using action_type = same_game::action_type;
  const int width = cfg.width;
  const int height = cfg.height;

  std::set<action_type> covered;
  std::vector<action_type> actions;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      short tile = state[x][y];
      if (tile == 0) {
        break;   
      }
      action_type cur(x, y);
      if (covered.count(cur)) {
        continue;
      }

      std::set<action_type> adjacent;
      find_adjacent(cfg, cur, adjacent, state);

      if (adjacent.size() > 1) {
        actions.push_back(cur);
        covered.insert(adjacent.begin(), adjacent.end());
        adjacent.erase(adjacent.find(std::make_pair(x,y)));
      }
    }
  }
  return actions;
}; 

template <class Game>
typename Game::state_type initialize_state_impl(const typename Game::config_type& cfg);

template <>
same_game::state_type initialize_state_impl<same_game>(
  const same_game::config_type& cfg
) {
  const int width = cfg.width;
  const int height = cfg.height;

  same_game::state_type state;

  using col_type = typename same_game::state_type::value_type;

  for (int i = 0; i < width; i++) {
    col_type col;
    for (int j = 0; j < height; j++) {
      col.push_back((std::rand() % 5) + 1);
    }
    state.push_back(col);
  }

  return state;
}

int get_num_children(const generic_game::config_type& cfg, int depth) {
  float lambda = cfg.nc_alpha + depth * cfg.nc_beta;
  std::poisson_distribution<int> distribution(lambda);
  return distribution(generator);
} 

std::vector<float> sample_uniform_dirichlet(float alpha, int n) {
  std::gamma_distribution<double> distribution(alpha, 1);

  std::vector<float> samples;
  for (int i = 0; i < n; i++) {
    samples.push_back(distribution(generator));
  }

  float sum = 0;
  for (auto& num : samples) {
    sum += num;
  }
  for (auto& num : samples) {
    num /= sum;
  }
  return samples;
}

template <class T>
float calc_variance(std::vector<T>& vec) {
  float mean = 0;
  for (auto& num : vec) {
    mean += num;
  }
  mean /= vec.size();

  float var = 0;
  for (auto& num : vec) {
    var += (num - mean) * (num - mean);
  }
  return var;
}

template <class T>
void print_vec(const std::vector<T>& vec) {
  for (auto& elem : vec) {
    std::cout << elem << " ";
  }
  std::cout << std::endl;
}

std::vector<float> get_child_means(
  const generic_game::config_type& cfg,
  float parent_mean,
  float parent_var,
  int parent_depth,
  int num_children
) {
  float alpha = cfg.disp_mean_delta + parent_depth * cfg.disp_mean_beta;

  std::vector<float> child_means;

  float coeff = num_children * parent_mean;

  while (true) {
    child_means = sample_uniform_dirichlet(alpha, num_children);
    for (auto& mean : child_means) {
      mean *= coeff;
    }
    std::cout << alpha << std::endl;
    print_vec(child_means);

    float child_mean_variance = calc_variance(child_means);
    std::cout << "child_mean_variance: " << child_mean_variance << std::endl;
    std::cout << "parent_var: " << parent_var << std::endl;

    if (calc_variance(child_means) < parent_var) {
      break;
    }
  }

  return child_means;
}

std::vector<float> get_child_vars(
  const generic_game::config_type& cfg,
  float parent_var,
  std::vector<float>& child_means,
  int parent_depth
) {
  float alpha = cfg.disp_var_delta + parent_depth * cfg.disp_var_beta;
  int num_children = child_means.size();
  std::vector<float> child_vars = sample_uniform_dirichlet(alpha, num_children);
  
  float coeff = num_children * (parent_var - calc_variance(child_means));
  for (auto& var : child_vars) {
    var *= coeff;
  }
  return child_vars;
}

bool is_failure(const generic_game::config_type& cfg) {
  std::bernoulli_distribution distribution(cfg.depth_p);
  return distribution(generator);
}

template <>
generic_game::state_type initialize_state_impl<generic_game>(
  const generic_game::config_type& cfg
) {
  generic_game::state_type state;
  state.depth = 0;
  state.fail_count = 0;
  state.mean = cfg.root_mean;
  state.var = cfg.root_var;
  state.is_terminal = false;

  state.num_children = get_num_children(cfg, state.depth);
  state.child_means = 
    get_child_means(cfg, state.mean, state.var, state.depth, state.num_children);
  state.child_vars = 
    get_child_vars(cfg, state.var, state.child_means, state.depth);

  return state;
}

template <>
std::vector<generic_game::action_type> get_moves_impl<generic_game>(
  const generic_game::config_type& cfg,
  const generic_game::state_type& state
) {
  std::vector<generic_game::action_type> actions;
  for (int i = 0; i < state.num_children; i++) {
    actions.push_back(i);
  }
  return actions;
}

template <>
float make_move_impl<generic_game>(
  const generic_game::config_type& cfg,
  const generic_game::action_type& action, 
  generic_game::state_type& state
) {
  state.depth++;
  bool is_fail_roll = is_failure(cfg);
  if (is_fail_roll) {
    state.fail_count++;
  }
  if (state.fail_count >= cfg.depth_r) {
    state.is_terminal = true;
  }
  state.mean = state.child_means[action];
  
  if (state.is_terminal) {
    state.var = 0;
    state.num_children = 0;
    state.child_means = {};
    state.child_vars = {};
  } else {
    state.var = state.child_vars[action];
    state.num_children = get_num_children(cfg, state.depth);
    state.child_means = 
      get_child_means(cfg, state.mean, state.var, state.depth, state.num_children);
    state.child_vars = 
      get_child_vars(cfg, state.var, state.child_means, state.depth);
  }

  return state.is_terminal ? state.mean : 0;
}

template <class Game>
class controller {
  public:
    using action_type = typename Game::action_type;
    using state_type = typename Game::state_type;
    using config_type = typename Game::config_type;
    std::vector<action_type> get_moves(const config_type& cfg, const state_type& state) const {
      return get_moves_impl<Game>(cfg, state);
    } 
    state_type initialize_state(const config_type& cfg) {
      return initialize_state_impl<Game>(cfg);
    }
    float make_move(const config_type& cfg, const action_type& action, state_type& state) {
      return make_move_impl<Game>(cfg, action, state);
    }
};
