#include <cstdlib>
#include <ctime>

#include "controller.hpp"
#include "game.hpp"
#include "monte_carlo_simulator.hpp"

int main() {
  std::srand(std::time(0));
  monte_carlo_simulator<same_game> mcs;
  mcs.simulate();
  return 0;
}

