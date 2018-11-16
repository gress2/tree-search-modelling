#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>
#include "game.hpp"

void find_adjacent(
    same_game::action_type action, 
    std::set<same_game::action_type>& adjacent,
    const same_game::state_type& state
) {

  using action_type = same_game::action_type;
  const int width = same_game::width;
  const int height = same_game::height;

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
    find_adjacent(up, adjacent, state);
  }

  if (!adjacent.count(right) && right.first < width && 
      state[right.first][right.second] == tile) {
    find_adjacent(right, adjacent, state);
  }

  if (!adjacent.count(down) && down.second >= 0 && 
      state[down.first][down.second] == tile) {
    find_adjacent(down, adjacent, state);
  }

  if (!adjacent.count(left) && left.first >= 0 && 
      state[left.first][left.second] == tile) {
    find_adjacent(left, adjacent, state);
  }
}

void collapse(same_game::state_type& state) {
  const int width = same_game::width;
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

bool is_game_over(const same_game::state_type& state) {
  const int width = same_game::width;
  const int height = same_game::height;

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

int num_tiles_remaining(const same_game::state_type& state) {
  const int width = same_game::width;
  const int height = same_game::height;

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
float make_move_impl(const typename Game::action_type& action, typename Game::state_type& state);

template <>
float make_move_impl<same_game>(const typename same_game::action_type& action, 
    typename same_game::state_type& state) {

  using action_type = typename same_game::action_type;

  std::set<action_type> adjacent;
  find_adjacent(action, adjacent, state);

  for (auto& elem : adjacent) {
    state[elem.first][elem.second] = 0;
  }

  collapse(state);

  int num_removed = adjacent.size();
  int reward = (num_removed - 2) * (num_removed - 2);

  if (is_game_over(state)) {
    int num_remaining = num_tiles_remaining(state);
    if (!num_remaining) {
      reward += 1000;
    } else {
      reward -= (num_remaining - 2) * (num_remaining - 2);
    }
  }
  return reward;
}

template <class Game>
std::vector<typename Game::action_type> get_moves_impl(const typename Game::state_type& state);

template <>
std::vector<same_game::action_type> get_moves_impl<same_game>(const typename same_game::state_type& state) {
  using action_type = typename same_game::action_type;
  const int width = same_game::width;
  const int height = same_game::height;

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
      find_adjacent(cur, adjacent, state);

      if (adjacent.size() > 1) {
        actions.push_back(cur);
        covered.insert(adjacent.begin(), adjacent.end());
        adjacent.erase(adjacent.find(std::make_pair(x,y)));
      }
    }
  }
  return actions;
}; 

template <class Config>
State initialize_state_impl(Config cfg);

template <>
typename same_game::state_type initialize_state_impl(typename same_game::config_type& cfg) {

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

template <class Game>
class controller {
  public:
    using action_type = typename Game::action_type;
    using state_type = typename Game::state_type;
    std::vector<action_type> get_moves(const state_type& state) const {
      return get_moves_impl<Game>(state);
    } 
    state_type initialize_state(config_type cfg) {
      return initialize_state_impl(cfg);
    }
    float make_move(action_type& action, state_type& state) {
      return make_move_impl<Game>(action, state);
    }
};
