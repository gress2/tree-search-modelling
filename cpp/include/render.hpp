#pragma once

template <class Game>
void render(
  const typename Game::config_type& cfg,
  const typename Game::state_type& state
);

template <>
void render<same_game>(
  const same_game::config_type& cfg, 
  const same_game::state_type& state
) {
  const int width = cfg.width;
  const int height = cfg.height;
  std::cout << "*********************************" << std::endl;
  for (int x = 0; x < width; x++) {
    std::cout << "| ";
    for (int y = 0; y < height; y++) {
      short tile = state[x][y];
      std::cout << "\033[9" << tile << "m" << tile << "\033[0m "; 
    }
    std::cout << "|" << std::endl;
  }
  std::cout << "*********************************" << std::endl;
}
