#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <stack>
#include <fstream>
#include <sstream>

#include "controller.hpp"

static std::size_t num_mcts_nodes = 0;

namespace search {
template <class Game>
struct node {
  using state_type = typename Game::state_type;
  state_type state;
  std::vector<typename Game::action_type> action_log;
  std::size_t depth;
  std::vector<node<Game>> children;
  bool is_terminal;
  node(state_type b)
    : state(b),
      depth(0),
      action_log({}),
      children({}),
      is_terminal(false)
  {}
  node(const node& other)
    : state(other.state),
      depth(other.depth),
      action_log(other.action_log),
      children(other.children),
      is_terminal(other.is_terminal)
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
  std::string game_name = Game::name;
  controller<Game> con;

  std::ofstream td_dist_f(game_name + "_td_dist"); 
  std::ofstream reward_dist_f(game_name + "_reward_dist");
  std::ofstream nc_dist_f(game_name + "_nc_dist");

  for (std::size_t i = 0; i < num_iters; i++) {
    state_type state = con.initialize_state(cfg); 
    auto moves = con.get_moves(cfg, state);
    int num_moves = 0;
    float reward = 0;
    while (!moves.empty()) {
      nc_dist_f << moves.size() << std::endl;
      int move_idx = std::rand() % moves.size();
      reward += con.make_move(cfg, moves[move_idx], state);
      num_moves++;
      moves = con.get_moves(cfg, state);
    }
    reward_dist_f << reward << std::endl;
    td_dist_f << (num_moves + 1) << std::endl;
    out_file << num_moves << std::endl;
  }
}

template <class Game>
struct mcts_node {
  using state_type = typename Game::state_type;
  using action_type = typename Game::action_type;

  state_type state;
  std::vector<mcts_node<Game>> children;
  bool is_terminal;
  std::size_t depth;
  std::size_t visit_count;
  float q;
  mcts_node* parent;
  std::size_t node_id;
  action_type action;

  mcts_node(state_type b)
    : state(b),
      children({}),
      is_terminal(false),
      depth(0),
      visit_count(0),
      q(0),
      parent(nullptr),
      node_id(num_mcts_nodes),
      action(Game::null_action)
  {}

  std::string to_gv_helper() const {
    std::stringstream ss;

    ss << " " << node_id << "[label=\"" << Game::state_to_str(state) << "\"]" << std::endl;
    for (auto& child : children) {
      ss << " " << node_id << " -- " << child.node_id << std::endl;
      ss << child.to_gv_helper() << std::endl;
    }
    return ss.str();
  }

  std::string to_gv() const {
    std::stringstream ss;
    ss << "graph {" << std::endl << to_gv_helper() << "}" << std::endl;
    return ss.str(); 
  }

};

template <class Game>
std::vector<typename Game::action_type> get_action_seq(mcts_node<Game>* v) {
  std::vector<typename Game::action_type> seq;
  while (v->parent) {
    seq.push_back(v->action);
    v = v->parent;
  }
  std::reverse(seq.begin(), seq.end());
  return seq;
} 

template <class Game>
float default_policy(mcts_node<Game>* v, controller<Game>& con,
    typename Game::config_type& cfg, float& high_score, 
    std::vector<typename Game::action_type>& hs_seq) {
  // std::cout << __PRETTY_FUNCTION__ << std::endl;

  using action_type = typename Game::action_type;
  using state_type = typename Game::state_type;

  state_type state = v->state;
  std::vector<action_type> seq;

  std::vector<action_type> moves = con.get_moves(cfg, state);

  float reward = 0;
  while (!moves.empty()) {
    int random_idx = std::rand() % moves.size();
    auto& move = moves[random_idx];
    seq.push_back(move);
    reward += con.make_move(cfg, move, state); 
    moves = con.get_moves(cfg, state);
  }

  if (reward > high_score) {
    high_score = reward;
    auto seq_to_v = get_action_seq(v);
    seq.insert(seq.end(), std::make_move_iterator(seq_to_v.begin()),
        std::make_move_iterator(seq_to_v.end()));
    hs_seq = seq;
  }

  return reward;
}

template <class Node>
Node* best_child(Node* v) {
  // std::cout << __PRETTY_FUNCTION__ << std::endl;
  std::size_t visit_parent = v->visit_count;

  constexpr float c = 1;
  float max_score = -std::numeric_limits<float>::max();
  Node* max_child = nullptr;

  for (auto& child : v->children) {
    float q_child = child.q;
    std::size_t visit_child = child.visit_count;
    if (!visit_child) {
      return &child;
    }
    float score = q_child / visit_child + c * std::sqrt(2 * std::log(visit_parent) / visit_child);
    if (score > max_score) {
      max_score = score;
      max_child = &child;
    }
  }

  return max_child;
}

template <class Game, class Controller, class Config>
mcts_node<Game>* tree_policy(mcts_node<Game>* v, Controller& con, Config& cfg) {
  // std::cout << __PRETTY_FUNCTION__ << std::endl;
  while (!v->is_terminal) {
    if (v->children.empty()) {
      auto moves = con.get_moves(cfg, v->state); 
      if (moves.empty()) {
        v->is_terminal = true;
        break;
      }
      for (auto& move : moves) {
        mcts_node<Game> cur(v->state); 
        cur.action = move;
        num_mcts_nodes++;
        cur.parent = v;
        cur.depth = v->depth + 1;
        con.make_move(cfg, move, cur.state);
        v->children.push_back(cur);
      }
      int rand_idx = std::rand() % v->children.size();
      return &(v->children[rand_idx]);
    } else {
      v = best_child(v); 
    }
  }
  return v;
}

template <class Game>
void backup(float reward, mcts_node<Game>* node) {
  // std::cout << __PRETTY_FUNCTION__ << std::endl;
  while (node) {
    node->q += reward;
    node->visit_count++;
    node = node->parent;
  }
} 

template <class Game>
void uct(
  typename Game::config_type cfg,
  std::size_t num_iters,
  std::ofstream& out_file
) {
  using state_type = typename Game::state_type;
  using node_type = mcts_node<Game>;
  using action_type = typename Game::action_type;
  controller<Game> con;
  std::string game_name = Game::name;

  float high_score = std::numeric_limits<float>::min();
  std::vector<action_type> hs_seq;

  node_type root(con.initialize_state(cfg)); 

  for (std::size_t i = 0; i < num_iters; i++) {
    node_type* v = tree_policy(&root, con, cfg);  
    float reward = default_policy(v, con, cfg, high_score, hs_seq);
    backup(reward, v);
  }

  std::cout << "hs: " << high_score << std::endl;
  std::cout << "seq: "; 
  for (auto& elem : hs_seq) {
    std::cout << Game::action_to_str(elem) << " ";
  }
  std::cout << std::endl;

  std::ofstream gv("tree.dot");
  gv << root.to_gv() << std::endl;
}
