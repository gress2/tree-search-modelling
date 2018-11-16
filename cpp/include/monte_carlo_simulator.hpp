#pragma once

#include <fstream>
#include <mutex>
#include <numeric> 
#include <thread>
#include "render.hpp"

static std::size_t id = 0;

template <class State>
struct node {
  State state;
  std::vector<node> children;
  std::size_t unique_id;
  float reward;
  bool simulated;
  bool direct;
  float mean;
  float var;
  std::size_t depth;

  node(State&& b) 
    : state(std::move(b)),
      reward(0),
      simulated(false),
      direct(false),
      mean(0),
      var(0),
      depth(0)
  {}

  node(const node& other)
   : state(other.state),
     unique_id(id++),
     reward(other.reward),
     simulated(other.simulated),
     mean(other.mean),
     var(other.var),
     depth(other.depth)
  {} 
};

template <class Game>
class monte_carlo_simulator {
  public:
    using state_type = typename Game::state_type;
    using node_type = node<state_type>;
  private:
    controller<Game> con_; 
    node_type root_;
    std::vector<node_type*> worklist_;
    std::size_t num_nodes_;
    std::size_t num_rollouts_;
    std::size_t num_unvisited_;
    std::mutex worklist_mutex_;
    std::ofstream mixing_data_f_;
  public:
    monte_carlo_simulator() 
      : root_(node_type(con_.initialize_state({state_type{}}))),
        worklist_({&root_}),
        num_nodes_(0),
        num_rollouts_(20),
        num_unvisited_(0),
        mixing_data_f_("mixing_data") 
    {}

    void rollout(node_type* node) {
      std::vector<float> rewards;
      for (std::size_t i = 0; i < num_rollouts_; i++) {
        node_type n(*node);
        float reward = n.reward;
        auto moves = con_.get_moves(n.state);
        while (moves.size()) {
          int random_idx = std::rand() % moves.size();
          reward += con_.make_move(moves[random_idx], n.state);
          n.depth++;
          moves = con_.get_moves(n.state);
        }
        rewards.push_back(reward);
      }
      float mean = std::accumulate(rewards.begin(), rewards.end(), 0) / static_cast<float>(rewards.size());
      float var = 0;

      std::for_each(rewards.begin(), rewards.end(), [&](const float r) {
        var += (r - mean) * (r - mean); 
      });

      var /= rewards.size() - 1; 

      std::lock_guard<std::mutex> lock(worklist_mutex_);
      node->reward = mean;
      node->simulated = true;
      node->mean = mean;
      node->var = var;
    }

    void rollout_range(std::vector<node_type*>& worklist, std::size_t start_idx, std::size_t end_idx) {
      for (std::size_t i = start_idx; i < worklist.size() && i < end_idx; i++) {
        rollout(worklist[i]);
      }
    }

    std::pair<float, float> mix(node_type* node) {
      if (node->children.empty()) {
        return std::make_pair(node->mean, node->var);
      } else {
        float sum = 0;
        int n = node->children.size();
        std::vector<std::pair<float, float>> c_vals;
        for (auto& child : node->children) {
          c_vals.push_back(mix(&child));
        }

        float mean = 0;
        float v1 = 0;
        float v2 = 0;
        for (std::pair<float, float>& p : c_vals) {
          mean += p.first / static_cast<float>(n);
          v1 += p.second / n;
          v2 += (p.first * p.first) / n; 
        }
        float var = v1 + v2 - (mean * mean);

        mixing_data_f_ << mean << " " << var << " " << node->depth << " ["; 

        auto it = c_vals.begin();
        mixing_data_f_ << "(" << it->first << "," << it->second << ")";
        ++it;
        while (it != c_vals.end()) {
          mixing_data_f_ << ", (" << it->first << "," << it->second << ")"; 
          ++it;
        } 
        mixing_data_f_ << "]" << std::endl;

        return std::make_pair(mean, var);
      } 
    }

    void simulate() {
      while (num_nodes_ < 1e5 && !worklist_.empty()) {
        int random_idx = std::rand() % worklist_.size(); 
        auto it = worklist_.begin();
        std::advance(it, random_idx);
        node_type* cur = *it;

        auto moves = con_.get_moves(cur->state);

        if (moves.empty()) {
          cur->var = 0;
          cur->mean = cur->reward;
          cur->direct = true;
        }

        for (auto& move : moves) {
          node<typename Game::state_type> _node(*cur);
          _node.reward += con_.make_move(move, _node.state);
          _node.depth++;
          cur->children.push_back(_node);
        }

        auto children = cur->children;
        num_nodes_ += children.size();

        for (auto& child : (*it)->children) {
          worklist_.push_back(&child);
        }

        auto beg = worklist_.begin();
        std::advance(beg, random_idx);
        worklist_.erase(beg);
      }

      std::vector<std::thread> threads;
      std::size_t num_threads = std::thread::hardware_concurrency();
      std::size_t workload_per_thread = worklist_.size() / num_threads + 1;

      std::cout << "Rolling out " << worklist_.size() << " non-terminal leaf nodes...";

      for (short i = 0; i < num_threads; i++) {
        std::size_t start_idx = workload_per_thread * i;
        std::size_t end_idx = start_idx + workload_per_thread;
        auto& wl = worklist_;
        threads.push_back(std::thread([this, &wl, start_idx, end_idx] { 
          this->rollout_range(wl, start_idx, end_idx); 
        }));
      }

      for (auto& thread : threads) {
        if (thread.joinable()) {
          thread.join();
        }
      }

      std::cout << "Done." << std::endl;

      std::cout << "Performing mixture model composition and writing data to disk...";
      auto pa = mix(&root_);
      std::cout << "Done." << std::endl;

      std::cout << "Root statistics --- (mean: " << pa.first << ", var: " << pa.second << ")" << std::endl; 
    }
};
