#pragma once

#include <iostream>
#include <sstream>
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

  constexpr static action_type null_action = std::make_pair(-1, -1);

  static void action_print(const action_type& action) {
    std::cout << "(" << action.first << ", " << action.second << ")"; 
  }

  static std::string action_to_str(const action_type& action) {
    std::stringstream ss;
    ss << "(" << action.first << ", " << action.second << ")";
    return ss.str();
  }

  static std::string state_to_str(const state_type& state) {
    return "<board>";
  }

  const static std::string name;
};

struct generic_game_state {
  float mean;
  float var;
  int succ_count;
  int depth;
  int num_children;
  std::vector<float> child_means;
  std::vector<float> child_vars;
  bool is_terminal;
};

struct generic_game_cfg {
  int depth_r;
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

  const static std::string name;

  constexpr static action_type null_action = -1;

  static std::string action_to_str(const action_type& action) {
    return std::to_string(action);  
  }

  static std::string state_to_str(const state_type& state) {
    return std::to_string(state.mean);
  }
};
