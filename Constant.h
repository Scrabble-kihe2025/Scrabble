
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Kích thước cửa sổ
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

// Kích thước bàn cờ
const int BOARD_X_OFFSET = 15;  // Vị trí bàn cờ trên màn hình
const int BOARD_Y_OFFSET = 15;
const int BOARD_PIXEL_SIZE = 600; // Kích thước ảnh bàn cờ
const int GRID_SIZE = 15; // 15x15 ô
const int TILE_SIZE = BOARD_PIXEL_SIZE / GRID_SIZE; // 40x40 pixel mỗi ô

// ... Các hằng số cũ

// Kích thước khay chữ
const int RACK_X = 150;
const int RACK_Y = 625;
const int RACK_WIDTH = 305;
const int RACK_HEIGHT = 50;
const int RACK_TILE_SIZE = 38; // Kích thước ô chữ trên khay
#endif // CONSTANTS_H
