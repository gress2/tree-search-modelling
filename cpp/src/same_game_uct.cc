#include <cstdlib>
#include <ctime>
#include <fstream>

#include "game.hpp"
#include "search.hpp"

int main() {
  std::srand(std::time(0));

  same_game::config_type cfg;
  cfg.height = 15;
  cfg.width = 15;

  std::ofstream out_f("uct");
  uct<same_game>(cfg, 3500, out_f);
}
