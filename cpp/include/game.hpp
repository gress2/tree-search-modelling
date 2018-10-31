#pragma once

#include <utility>
#include <vector>

struct same_game {
  using board_type = std::vector<std::vector<short>>;
  using action_type = std::pair<short, short>; 
  static const int width = 15;
  static const int height = 15;
};
