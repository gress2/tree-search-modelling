#pragma once

#include <iostream>
#include <utility>
#include <vector>

struct same_game_cfg {
  int width;
  int height;
};

struct same_game {
  using state_type = std::vector<std::vector<short>>;
  using action_type = std::pair<short, short>; 
  using config_type = same_game_cfg;
  static void action_print(const action_type& action) {
    std::cout << "(" << action.first << ", " << action.second << ")"; 
  }
};

struct generic_game_state {
  float mean;
  float var;
  int fail_count;
  int depth;
  int num_children;
  std::vector<float> child_means;
  std::vector<float> child_vars;
};

struct generic_game_cfg {
  float depth_r;
  float depth_p;
  float disp_mean_delta;
  float disp_mean_beta;
  float disp_var_delta;
  float disp_var_beta;
  float nc_alpha;
  float nc_beta;
  float root_mean;
  float root_var;
};

struct generic_game {
  using state_type = generic_game_state;
  using action_type = int;
  using config_type = generic_game_cfg;
};
