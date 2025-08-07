// File: Board.h (Phiên bản cập nhật)
#ifndef BOARD_H
#define BOARD_H

#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include "Constant.h"
#include "Tile.h"

enum class Bonus {
    NONE,
    DOUBLE_LETTER,
    TRIPLE_LETTER,
    DOUBLE_WORD,
    TRIPLE_WORD
};

class Board {
public:
    Board(SDL_Renderer* renderer);
    ~Board();

    // Sửa hàm render để nhận vào map texture
    void render(const std::map<char, SDL_Texture*>& tileTextures);
    void placeTile(Tile tile, int row, int col);
    Tile getTileAt(int row, int col);
    void removeTile(int row, int col);
    Bonus getBonusAt(int row, int col) const;

private:
    bool loadBoardTexture(); // Chỉ tải texture của bàn cờ
    void renderGrid();
    // Sửa hàm renderTiles để nhận vào map texture
    void renderTiles(const std::map<char, SDL_Texture*>& tileTextures);

    SDL_Renderer* m_renderer;
    Tile m_grid[GRID_SIZE][GRID_SIZE];
    Bonus m_bonusGrid[15][15];
    SDL_Texture* m_boardTexture;
};

#endif // BOARD_H
