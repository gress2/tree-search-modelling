#pragma once

#include <random>
#include <stack>

#include "controller.hpp"

namespace search {
template <class Game>
struct node {
  using state_type = typename Game::state_type;
  state_type state;
  std::vector<typename Game::action_type> action_log;
  std::size_t depth;
  node(state_type b)
    : state(b),
      depth(0),
      action_log({})
  {}
  node(const node& other)
    : state(other.state),
      depth(other.depth),
      action_log(other.action_log)
  {}
  void print_action_log() {
    std::cout << depth << std::endl;
    std::cout << action_log.size() << std::endl;
    for (auto& action : action_log) {
      Game::action_print(action);
      std::cout << " ";
    }
    std::cout << std::endl;
  }
};
}

template <class Game>
void depth_first_search(
  std::size_t num_restarts, 
  std::size_t max_iters,
  std::ofstream& out_file
) {
  using state_type = typename Game::state_type;
  using node_type = search::node<Game>;
  
  controller<Game> con;
  std::random_device rd;
  std::mt19937 twister(rd());

  for (std::size_t i = 0; i < num_restarts; i++) {
    std::stack<node_type> stck;
    node_type top(con.initialize_state(state_type{}));
    stck.push(top);
    std::size_t iter_cnt = 0;

    while (!stck.empty() && iter_cnt < max_iters) {
      node_type cur = stck.top();
      stck.pop();
      auto moves = con.get_moves(cur.state);
      if (moves.empty()) {
        out_file << cur.depth << std::endl;
      }
      std::shuffle(moves.begin(), moves.end(), twister);

      for (auto& move : moves) {
        node_type child(cur);
        con.make_move(move, child.state);
        child.depth++;
        stck.push(child);
        iter_cnt++;
      }
    }
  }
}

template <class Game>
void random_search(
  typename Game::config_type cfg,
  std::size_t num_iters, 
  std::ofstream& out_file
) {
  using state_type = typename Game::state_type;
  controller<Game> con;

  for (std::size_t i = 0; i < num_iters; i++) {
    state_type state = con.initialize_state(cfg); 
    auto moves = con.get_moves(cfg, state);
    int num_moves = 0;
    while (!moves.empty()) {
      int move_idx = std::rand() % moves.size();
      con.make_move(cfg, moves[move_idx], state);
      num_moves++;
      moves = con.get_moves(cfg, state);
    }
    out_file << num_moves << std::endl;
  }
}