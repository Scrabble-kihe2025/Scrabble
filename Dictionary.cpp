
#include "Dictionary.h"
#include <fstream>   // Để đọc file
#include <iostream>
#include <algorithm> // Để dùng std::transform

Dictionary::Dictionary(const std::string& filename) {
    std::ifstream file(filename); // Mở file để đọc

    if (!file.is_open()) {
        std::cout << "ERROR: Could not open dictionary file: " << filename << std::endl;
        return;
    }

    std::string word;
    // Đọc từng dòng (mỗi dòng là một từ) trong file
    while (std::getline(file, word)) {
        // Chuyển từ thành chữ HOA để việc so sánh không phân biệt hoa thường
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);

        // Loại bỏ ký tự '\r' có thể có ở cuối dòng trong file Windows
        if (!word.empty() && word.back() == '\r') {
            word.pop_back();
        }

        if (!word.empty()) {
             m_wordSet.insert(word);
        }
    }

    file.close();
    std::cout << "Dictionary loaded. Total words: " << m_wordSet.size() << std::endl;
}

bool Dictionary::isWordValid(const std::string& word) const {
    // std::unordered_set::count() trả về 1 nếu tìm thấy, 0 nếu không.
    // Rất hiệu quả!
    return m_wordSet.count(word) > 0;
}
