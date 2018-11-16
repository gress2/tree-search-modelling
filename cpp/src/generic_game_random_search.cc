#include <cstdlib>
#include <ctime>
#include <fstream>

#include "game.hpp"
#include "search.hpp"

int main() {
  std::srand(std::time(0));
  std::ofstream out_file("depths");


  using game_type = generic_game<

  >;

  random_search<game_type>(1e5, out_file);
}