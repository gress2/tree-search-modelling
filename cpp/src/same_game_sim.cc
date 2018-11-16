#include <cstdlib>
#include <ctime>

#include "controller.hpp"
#include "game.hpp"
#include "monte_carlo_simulator.hpp"

int main() {
  std::srand(std::time(0));

  same_game::config_type cfg;
  cfg.height = 15;
  cfg.width = 15;

  monte_carlo_simulator<same_game> mcs(cfg);
  mcs.simulate();
  return 0;
}

