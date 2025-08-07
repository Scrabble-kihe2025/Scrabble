#include "Board.h"
#include <iostream>

// Hàm loadTexture này không còn cần thiết trong file này nữa, vì lớp Game sẽ quản lý.
// Nhưng để nó ở đây cũng không sao.

Board::Board(SDL_Renderer* renderer) : m_renderer(renderer) {
    // Khởi tạo lưới logic, tất cả các ô ban đầu là trống
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            m_grid[i][j] = {' ', 0};
            m_bonusGrid[i][j] = Bonus::NONE; // Ô trống
        }
    }

     m_bonusGrid[0][0] = m_bonusGrid[0][7] = m_bonusGrid[0][14] = Bonus::TRIPLE_WORD;
    m_bonusGrid[7][0] = m_bonusGrid[7][14] = Bonus::TRIPLE_WORD;
    m_bonusGrid[14][0] = m_bonusGrid[14][7] = m_bonusGrid[14][14] = Bonus::TRIPLE_WORD;

    // Thiết lập các ô Double Word (DW)
    m_bonusGrid[1][1] = m_bonusGrid[2][2] = m_bonusGrid[3][3] = m_bonusGrid[4][4] = Bonus::DOUBLE_WORD;
    m_bonusGrid[1][13] = m_bonusGrid[2][12] = m_bonusGrid[3][11] = m_bonusGrid[4][10] = Bonus::DOUBLE_WORD;
    m_bonusGrid[13][1] = m_bonusGrid[12][2] = m_bonusGrid[11][3] = m_bonusGrid[10][4] = Bonus::DOUBLE_WORD;
    m_bonusGrid[13][13] = m_bonusGrid[12][12] = m_bonusGrid[11][11] = m_bonusGrid[10][10] = Bonus::DOUBLE_WORD;
    // Ô trung tâm cũng là Double Word
    m_bonusGrid[7][7] = Bonus::DOUBLE_WORD;

    // Thiết lập các ô Triple Letter (TL)
    m_bonusGrid[1][5] = m_bonusGrid[1][9] = Bonus::TRIPLE_LETTER;
    m_bonusGrid[5][1] = m_bonusGrid[5][5] = m_bonusGrid[5][9] = m_bonusGrid[5][13] = Bonus::TRIPLE_LETTER;
    m_bonusGrid[9][1] = m_bonusGrid[9][5] = m_bonusGrid[9][9] = m_bonusGrid[9][13] = Bonus::TRIPLE_LETTER;
    m_bonusGrid[13][5] = m_bonusGrid[13][9] = Bonus::TRIPLE_LETTER;

    // Thiết lập các ô Double Letter (DL)
    m_bonusGrid[0][3] = m_bonusGrid[0][11] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[2][6] = m_bonusGrid[2][8] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[3][0] = m_bonusGrid[3][7] = m_bonusGrid[3][14] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[6][2] = m_bonusGrid[6][6] = m_bonusGrid[6][8] = m_bonusGrid[6][12] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[7][3] = m_bonusGrid[7][11] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[8][2] = m_bonusGrid[8][6] = m_bonusGrid[8][8] = m_bonusGrid[8][12] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[11][0] = m_bonusGrid[11][7] = m_bonusGrid[11][14] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[12][6] = m_bonusGrid[12][8] = Bonus::DOUBLE_LETTER;
    m_bonusGrid[14][3] = m_bonusGrid[14][11] = Bonus::DOUBLE_LETTER;
    // --- KẾT THÚC THÊM MỚI ---

    // SỬA Ở ĐÂY: Gọi đúng tên hàm mới
    if (!loadBoardTexture()) {
        std::cout << "Failed to load board textures!" << std::endl;
    }
}

Board::~Board() {
    // SỬA Ở ĐÂY: Chỉ dọn dẹp texture của bàn cờ.
    // Board không còn quản lý m_tileTextures nữa.
    SDL_DestroyTexture(m_boardTexture);
}

// SỬA Ở ĐÂY: Định nghĩa hàm này phải khớp với file .h
void Board::render(const std::map<char, SDL_Texture*>& tileTextures) {
    renderGrid();
    renderTiles(tileTextures);
}

void Board::placeTile(Tile tile, int row, int col) {
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        m_grid[row][col] = tile;
    }
}

// Hàm này đã đúng, không cần sửa
bool Board::loadBoardTexture() {
    m_boardTexture = IMG_LoadTexture(m_renderer, "assets/board.png");
    if (m_boardTexture == nullptr) {
        std::cout << "Failed to load board.png: " << IMG_GetError() << std::endl;
        return false;
    }
    return true;
}

// Hàm này đã đúng, không cần sửa
void Board::renderGrid() {
    SDL_Rect destRect = { BOARD_X_OFFSET, BOARD_Y_OFFSET, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE };
    SDL_RenderCopy(m_renderer, m_boardTexture, NULL, &destRect);
}

// SỬA Ở ĐÂY: Định nghĩa hàm này phải khớp với file .h, bao gồm cả tham số
void Board::renderTiles(const std::map<char, SDL_Texture*>& tileTextures) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            char letter = m_grid[row][col].letter;
            if (letter != ' ') { // Nếu ô này có chữ
                SDL_Rect destRect;
                destRect.x = BOARD_X_OFFSET + col * TILE_SIZE;
                destRect.y = BOARD_Y_OFFSET + row * TILE_SIZE;
                destRect.w = TILE_SIZE;
                destRect.h = TILE_SIZE;

                // Tìm texture tương ứng và vẽ nó
                // 'tileTextures' ở đây chính là tham số được truyền vào
                auto it = tileTextures.find(letter);
                if (it != tileTextures.end()) {
                    SDL_RenderCopy(m_renderer, it->second, NULL, &destRect);
                }
            }
        }
    }
}

Tile Board::getTileAt(int row, int col) {
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        return m_grid[row][col];
    }
    return {'\0', -1}; // Trả về một tile không hợp lệ nếu vị trí ngoài bàn cờ
}
// Thêm hàm này vào cuối file Board.cpp
void Board::removeTile(int row, int col) {
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        m_grid[row][col] = {' ', 0}; // Đặt lại thành ô trống
    }
}

Bonus Board::getBonusAt(int row, int col) const {
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
        return m_bonusGrid[row][col];
    }
    return Bonus::NONE;
}
