
#ifndef TILEBAG_H
#define TILEBAG_H

#include <vector>
#include <random>
#include "Tile.h"

class TileBag {
public:
    TileBag(); // Constructor để khởi tạo 100 ô chữ
    void shuffle(); // Xáo trộn các ô
    Tile drawTile(); // Rút một ô
    bool isEmpty();
    int remainingTiles();

private:
    std::vector<Tile> m_tiles;
    std::mt19937 m_rng; // Mersenne Twister random number generator
};

#endif // TILEBAG_H
