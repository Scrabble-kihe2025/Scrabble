
#include "TileBag.h"
#include <algorithm>
#include <chrono>
#include<tuple>
TileBag::TileBag() {
    // Khởi tạo bộ sinh số ngẫu nhiên với seed từ đồng hồ hệ thống
    m_rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    // Phân phối chữ cái và điểm số chuẩn của Scrabble tiếng Anh
    // (Số lượng, Ký tự, Điểm)
    std::vector<std::tuple<int, char, int>> distribution = {
        {9, 'A', 1}, {2, 'B', 3}, {2, 'C', 3}, {4, 'D', 2}, {12, 'E', 1},
        {2, 'F', 4}, {3, 'G', 2}, {2, 'H', 4}, {9, 'I', 1}, {1, 'J', 8},
        {1, 'K', 5}, {4, 'L', 1}, {2, 'M', 3}, {6, 'N', 1}, {8, 'O', 1},
        {2, 'P', 3}, {1, 'Q', 10}, {6, 'R', 1}, {4, 'S', 1}, {6, 'T', 1},
        {4, 'U', 1}, {2, 'V', 4}, {2, 'W', 4}, {1, 'X', 8}, {2, 'Y', 4},
        {1, 'Z', 10}, {2, ' ', 0} // 2 ô trống (Blank tiles)
    };

    for (const auto& d : distribution) {
        for (int i = 0; i < std::get<0>(d); ++i) {
            m_tiles.push_back({std::get<1>(d), std::get<2>(d)});
        }
    }
}

void TileBag::shuffle() {
    std::shuffle(m_tiles.begin(), m_tiles.end(), m_rng);
}

Tile TileBag::drawTile() {
    if (isEmpty()) {
        return {' ', 0}; // Trả về ô rỗng nếu túi đã hết
    }
    Tile t = m_tiles.back();
    m_tiles.pop_back();
    return t;
}

bool TileBag::isEmpty() {
    return m_tiles.empty();
}

int TileBag::remainingTiles() {
    return m_tiles.size();
}
