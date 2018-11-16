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

struct v0_state {
  float mean;
  float var;
  int fail_count;
  int depth;
};

struct v0_cfg {
  
};

struct v0_game {
  using state_type = v0_state;
  using action_type = void;
  using config_type = v0_cfg;
};
