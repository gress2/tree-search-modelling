#include <game.hpp>
#include <search.hpp>

int main() {
  generic_game::config_type cfg;
  std::ofstream out_f("uct"); 

  uct<generic_game>(cfg, 100, out_f); 
}
