#include <cstdlib>
#include <ctime>
#include <fstream>

#include "game.hpp"
#include "search.hpp"

int main() {
  std::srand(std::time(0));
  std::ofstream out_file("depths");
  
  same_game::config_type cfg;
  cfg.height = 15;
  cfg.width = 15;

  random_search<same_game>(cfg, 1e5, out_file);
}