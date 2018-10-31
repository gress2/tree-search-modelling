#pragma once

#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>
#include "game.hpp"

void find_adjacent(
    same_game::action_type action, 
    std::set<same_game::action_type>& adjacent,
    const same_game::board_type& board
) {

  using action_type = same_game::action_type;
  const int width = same_game::width;
  const int height = same_game::height;

  adjacent.insert(action);
  short x = action.first;
  short y = action.second;
  
  short tile = board[x][y];

  action_type up(x, y + 1);
  action_type right(x + 1, y);
  action_type down(x, y - 1);
  action_type left(x - 1, y);

  if (!adjacent.count(up) && up.second < width && board[up.first][up.second] == tile) {
    find_adjacent(up, adjacent, board);
  }

  if (!adjacent.count(right) && right.first < width && board[right.first][right.second] == tile) {
    find_adjacent(right, adjacent, board);
  }

  if (!adjacent.count(down) && down.second >= 0 && board[down.first][down.second] == tile) {
    find_adjacent(down, adjacent, board);
  }

  if (!adjacent.count(left) && left.first >= 0 && board[left.first][left.second] == tile) {
    find_adjacent(left, adjacent, board);
  }
}

void collapse(same_game::board_type& board) {
  const int width = same_game::width;
  for (int x = 0; x < width; x++) {
    std::vector<short>& col = board[x];
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

  for (auto it = board.begin(); it != board.end();) {
    std::vector<short>& col = *it;
    bool all_zero = std::all_of(col.begin(), col.end(),
        [](short i) { return i == 0; });
    if (all_zero) {
      it = board.erase(it);
    } else {
      ++it;
    }
  }

  int num_to_fill = width - board.size();
  std::vector<short> to_fill(width, 0);
  board.insert(board.end(), num_to_fill, to_fill);
}

bool is_game_over(const same_game::board_type& board) {
  const int width = same_game::width;
  const int height = same_game::height;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      short tile = board[x][y];
      if (tile == 0) {
        break;
      }
      if (y + 1 < width && board[x][y] == tile) {
        return false;
      }
      if (x + 1 < width && board[x][y] == tile) {
        return false;
      }
    }
  }
  return true;
}

bool is_board_empty(const same_game::board_type& board) {
  for (auto& col : board) {
    bool all_zero = std::all_of(col.begin(), col.end(),
        [](short i) { return i == 0; });
    if (!all_zero) {
      return false;
    }
  }
  return true;
}

template <class Game>
void make_move_impl(const typename Game::action_type& action, typename Game::board_type& board);

template <>
void make_move_impl<same_game>(const typename same_game::action_type& action, 
    typename same_game::board_type& board) {

  using action_type = typename same_game::action_type;

  std::set<action_type> adjacent;
  find_adjacent(action, adjacent, board);

  for (auto& elem : adjacent) {
    board[elem.first][elem.second] = 0;
  }

  collapse(board);

  int num_removed = adjacent.size();
  int reward = (num_removed - 2) * (num_removed - 2);

  if (is_game_over(board)) {
    if (is_board_empty(board)) {

    }
  }

}

template <class Game>
std::vector<typename Game::action_type> get_moves_impl(const typename Game::board_type& board);

template <>
std::vector<same_game::action_type> get_moves_impl<same_game>(const typename same_game::board_type& board) {
  using action_type = typename same_game::action_type;
  const int width = same_game::width;
  const int height = same_game::height;

  std::set<action_type> covered;
  std::vector<action_type> actions;

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      short tile = board[x][y];
      if (tile == 0) {
        break;   
      }
      action_type cur(x, y);
      if (covered.count(cur)) {
        continue;
      }

      std::set<action_type> adjacent;
      find_adjacent(cur, adjacent, board);

      if (adjacent.size() > 1) {
        actions.push_back(cur);
        covered.insert(adjacent.begin(), adjacent.end());
        adjacent.erase(adjacent.find(std::make_pair(x,y)));
      }
    }
  }
  return actions;
}; 

template <class Board>
Board initialize_board_impl(Board board);

template <>
typename same_game::board_type initialize_board_impl(typename same_game::board_type board) {
  std::cout << "Initializing root board..." << std::endl;
  std::srand(32);

  const int width = same_game::width;
  const int height = same_game::height;

  using col_type = typename same_game::board_type::value_type;

  for (int i = 0; i < width; i++) {
    col_type col;
    for (int j = 0; j < height; j++) {
      col.push_back((std::rand() % 5) + 1);
    }
    board.push_back(col);
  }

  return board;
}

template <class Game>
class controller {
  public:
    using action_type = typename Game::action_type;
    using board_type = typename Game::board_type;
    std::vector<action_type> get_moves(const board_type& board) const {
      return get_moves_impl<Game>(board);
    } 
    board_type initialize_board(board_type board) {
      return initialize_board_impl(board);
    }
    void make_move(action_type& action, board_type& board) {
      make_move_impl<Game>(action, board);
    }
};
