#include <game.hpp>
#include <search.hpp>

int main() {
  generic_game::config_type cfg;
  cfg.depth_r = 130;
  cfg.depth_p = 1 - .3443;
  cfg.disp_mean_delta = 108.382;
  cfg.disp_mean_beta = 3.44;
  cfg.disp_var_delta = 4;
  cfg.disp_var_beta = .0051;
  cfg.nc_alpha = 1;
  // cfg.nc_alpha = 3.966;
  cfg.nc_beta = -.0346;
  cfg.root_mean = 213.493;
  cfg.root_var = 4273.46;  
 
  std::ofstream out_f("uct"); 

  uct<generic_game>(cfg, 350, out_f); 
}
