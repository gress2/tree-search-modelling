#pragma once

template <class Board>
void render(const Board& board);

template <>
void render(const same_game::board_type& board) {
  const int width = same_game::width;
  const int height = same_game::height;
  std::cout << "*********************************" << std::endl;
  for (int x = 0; x < width; x++) {
    std::cout << "| ";
    for (int y = 0; y < height; y++) {
      short tile = board[x][y];
      std::cout << "\033[9" << tile << "m" << tile << "\033[0m "; 
    }
    std::cout << "|" << std::endl;
  }
  std::cout << "*********************************" << std::endl;
}
