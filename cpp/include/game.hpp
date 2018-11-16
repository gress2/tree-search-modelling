#pragma once

#include <iostream>
#include <utility>
#include <vector>

struct same_game {
  using state_type = std::vector<std::vector<short>>;
  using action_type = std::pair<short, short>; 
  static const int width = 15;
  static const int height = 15;
  static void action_print(const action_type& action) {
    std::cout << "(" << action.first << ", " << action.second << ")"; 
  }
};

template <class DepthFn, class ArityFn, class DispersionFn>
struct generic_game {
  using state_type = std::pair<float, float>; // mean and variance of node
  using action_type = void;
};