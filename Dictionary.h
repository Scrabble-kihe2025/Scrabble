#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <unordered_set> // Dùng cấu trúc này để tìm kiếm từ cực nhanh

class Dictionary {
public:
    // Constructor sẽ nhận vào đường dẫn của file từ điển
    Dictionary(const std::string& filename);

    // Hàm để kiểm tra một từ có hợp lệ không
    bool isWordValid(const std::string& word) const;

private:
    std::unordered_set<std::string> m_wordSet;
};

#endif // DICTIONARY_H
