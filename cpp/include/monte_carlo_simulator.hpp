#pragma once

#include <unordered_set>
#include "render.hpp"

static std::size_t id = 0;

template <class Board>
struct node {
  Board board;
  std::vector<node> children;
  std::size_t unique_id;

  node(const Board& other)
   : board(other),
     unique_id(id++) 
  {} 
};

struct node_hash {
  template <class Node>
  std::size_t operator() (const Node& node) const {
    return node.unique_id;
  } 
};

struct node_equality {
  template <class Node>
  bool operator() (const Node& lhs, const Node& rhs) const {
    return lhs.unique_id == rhs.unique_id;
  }
};

template <class Game>
class monte_carlo_simulator {
  public:
    using board_type = typename Game::board_type;
    using node_type = node<board_type>;
  private:
    controller<Game> con_; 
    node_type root_;
    std::unordered_set<node_type, node_hash, node_equality> worklist_;
  public:
    monte_carlo_simulator() 
      : root_(node_type(con_.initialize_board({board_type{}}))),
        worklist_({root_}) {
      render(root_.board);
      
      auto moves = con_.get_moves(root_.board);
      con_.make_move(moves[0], root_.board);

    }

};
