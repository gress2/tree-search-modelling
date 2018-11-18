#include <cstdlib>
#include <ctime>
#include <fstream>

#include "game.hpp"
#include "search.hpp"

int main() {
  std::srand(std::time(0));
  std::ofstream out_file("depths");

  generic_game::config_type cfg;
  cfg.depth_r = 128;
  cfg.depth_p = 1 - .3455;
  cfg.disp_mean_delta = 108.382;
  cfg.disp_mean_beta = 3.44;
  cfg.disp_var_delta = 4;
  cfg.disp_var_beta = .0051;
  cfg.nc_alpha = 46.323;
  cfg.nc_beta = -.738;
  cfg.root_mean = 213.493;
  cfg.root_var = 19000;//4273.46;  

  random_search<generic_game>(cfg, 1e3, out_file);
}