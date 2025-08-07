#include "Game.h"
#include "Constant.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "TileBag.h"

// Hàm trợ giúp (giữ nguyên)
SDL_Texture* createTextTexture(TTF_Font* font, const std::string& text, SDL_Color color, SDL_Renderer* renderer) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) { return nullptr; }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Hàm khởi tạo (đã sửa)
Game::Game() : m_isRunning(false), m_window(nullptr), m_renderer(nullptr),
               m_board(nullptr), m_tileBag(nullptr), m_dictionary(nullptr),
               m_font(nullptr), m_menuBackgroundTexture(nullptr),
               m_currentPlayer(1), // Khởi tạo người chơi 1
               m_player1Score(0), m_player2Score(0),
               m_isFirstMove(true),
               m_isDragging(false), m_draggedTileOriginalIndex(-1)
{}

Game::~Game() {
    cleanUp();
}

// Hàm init() và initMenu() (giữ nguyên)
bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    m_window = SDL_CreateWindow("My Scrabble Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!m_window) return false;
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) return false;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;
    if (TTF_Init() == -1) return false;
    if (!initMenu()) return false;
    m_currentState = GameState::MAIN_MENU;
    m_isRunning = true;
    return true;
}

bool Game::initMenu() {
    m_font = TTF_OpenFont("assets/fonts/arial.ttf", 48);
    if (!m_font) return false;
    m_menuBackgroundTexture = IMG_LoadTexture(m_renderer, "assets/background.png");
    if (!m_menuBackgroundTexture) return false;
    m_textColor = {255, 255, 255, 255};
    m_menuTextures["pvp"] = createTextTexture(m_font, "1 vs 1", m_textColor, m_renderer);
    m_menuTextures["ai"] = createTextTexture(m_font, "1 vs AI", m_textColor, m_renderer);
    m_menuTextures["guide"] = createTextTexture(m_font, "Guide", m_textColor, m_renderer);
    int w, h;
    SDL_QueryTexture(m_menuTextures["pvp"], NULL, NULL, &w, &h);
    m_menuRects["pvp"] = {(SCREEN_WIDTH - w) / 2, 250, w, h};
    SDL_QueryTexture(m_menuTextures["ai"], NULL, NULL, &w, &h);
    m_menuRects["ai"] = {(SCREEN_WIDTH - w) / 2, 350, w, h};
    SDL_QueryTexture(m_menuTextures["guide"], NULL, NULL, &w, &h);
    m_menuRects["guide"] = {(SCREEN_WIDTH - w) / 2, 450, w, h};
    return true;
}


void Game::run() {
    while (m_isRunning) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) m_isRunning = false;
        switch (m_currentState) {
            case GameState::MAIN_MENU: handleMenuEvents(e); break;
            case GameState::PLAYING_PVP:
            case GameState::PLAYING_AI: handleGameEvents(e); break;
            default: break;
        }
    }
}

void Game::handleMenuEvents(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        SDL_Point mousePoint = {x, y};
        if (SDL_PointInRect(&mousePoint, &m_menuRects["pvp"])) {
            if (initGameAssets()) m_currentState = GameState::PLAYING_PVP;
        }
        else if (SDL_PointInRect(&mousePoint, &m_menuRects["ai"])) {
            if (initGameAssets()) m_currentState = GameState::PLAYING_AI;
        }
    }
}

// Hàm handleGameEvents (đã sửa)
void Game::handleGameEvents(SDL_Event& e) {
    auto& currentRack = (m_currentPlayer == 1) ? m_player1Rack : m_player2Rack;

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (m_isDragging) return;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mousePoint = {mouseX, mouseY};

        if (SDL_PointInRect(&mousePoint, &m_gameButtonRects["submit"])) { handleSubmit(); return; }
        if (SDL_PointInRect(&mousePoint, &m_gameButtonRects["recall"])) { recallTiles(); return; }
        if (SDL_PointInRect(&mousePoint, &m_gameButtonRects["pass"])) { /* ... */ return; }

        for (size_t i = 0; i < currentRack.size(); ++i) { // SỬA: Dùng currentRack
            if (currentRack[i].letter != '\0') {
                SDL_Rect tileRect = { RACK_X + 5 + (int)i * (RACK_TILE_SIZE + 5), RACK_Y + (RACK_HEIGHT - RACK_TILE_SIZE) / 2, RACK_TILE_SIZE, RACK_TILE_SIZE };
                if (SDL_PointInRect(&mousePoint, &tileRect)) {
                    m_isDragging = true;
                    m_draggedTile = currentRack[i]; // SỬA: Dùng currentRack
                    m_draggedTileOriginalIndex = i;
                    m_dragOffsetX = mouseX - tileRect.x;
                    m_dragOffsetY = mouseY - tileRect.y;
                    currentRack.erase(currentRack.begin() + i); // SỬA: Dùng currentRack
                    break;
                }
            }
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP) {
        if (m_isDragging) {
            m_isDragging = false;
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point mousePoint = {mouseX, mouseY};
            SDL_Rect boardRect = {BOARD_X_OFFSET, BOARD_Y_OFFSET, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE};

            bool placedOnBoard = false;
            if (SDL_PointInRect(&mousePoint, &boardRect)) {
                int col = (mouseX - BOARD_X_OFFSET) / TILE_SIZE;
                int row = (mouseY - BOARD_Y_OFFSET) / TILE_SIZE;

                if (m_board->getTileAt(row, col).letter == ' ') {
                    m_board->placeTile(m_draggedTile, row, col);
                    m_tilesOnBoardThisTurn.push_back({m_draggedTile, row, col});
                    placedOnBoard = true;
                }
            }

            if (!placedOnBoard) {
                currentRack.insert(currentRack.begin() + m_draggedTileOriginalIndex, m_draggedTile); // SỬA: Dùng currentRack
            } else {
                std::sort(m_tilesOnBoardThisTurn.begin(), m_tilesOnBoardThisTurn.end(),
                    [](const PlacedTile& a, const PlacedTile& b) {
                        if (a.row != b.row) return a.row < b.row;
                        return a.col < b.col;
                    });
            }
        }
    }
}

void Game::update() {}

void Game::render() {
    SDL_SetRenderDrawColor(m_renderer, 30, 30, 30, 255);
    SDL_RenderClear(m_renderer);
    switch (m_currentState) {
        case GameState::MAIN_MENU: renderMenu(); break;
        case GameState::PLAYING_PVP:
        case GameState::PLAYING_AI: renderGame(); break;
        default: break;
    }
    SDL_RenderPresent(m_renderer);
}

void Game::renderMenu() {
    SDL_RenderCopy(m_renderer, m_menuBackgroundTexture, NULL, NULL);
    SDL_RenderCopy(m_renderer, m_menuTextures["pvp"], NULL, &m_menuRects["pvp"]);
    SDL_RenderCopy(m_renderer, m_menuTextures["ai"], NULL, &m_menuRects["ai"]);
    SDL_RenderCopy(m_renderer, m_menuTextures["guide"], NULL, &m_menuRects["guide"]);
}

// Hàm renderGame (đã sửa)
void Game::renderGame() {
    SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255);
    SDL_RenderClear(m_renderer);

    if (m_board) {
        // Tạm thời chỉ dùng bộ texture P1 để vẽ bàn cờ.
        // Một giải pháp hoàn chỉnh sẽ phức tạp hơn.
        m_board->render(m_tileTexturesP1);
    }
    renderPlayerRack();

    if (!m_gameButtonTextures.empty()) {
        SDL_RenderCopy(m_renderer, m_gameButtonTextures["submit"], NULL, &m_gameButtonRects["submit"]);
        SDL_RenderCopy(m_renderer, m_gameButtonTextures["recall"], NULL, &m_gameButtonRects["recall"]);
        SDL_RenderCopy(m_renderer, m_gameButtonTextures["pass"], NULL, &m_gameButtonRects["pass"]);
    }

    renderDraggedTile();
}

// Hàm renderPlayerRack (đã sửa)
void Game::renderPlayerRack() {
    SDL_Rect rackRect = { RACK_X, RACK_Y, RACK_WIDTH, RACK_HEIGHT };
    SDL_SetRenderDrawColor(m_renderer, 50, 30, 10, 255);
    SDL_RenderFillRect(m_renderer, &rackRect);

    auto& currentRack = (m_currentPlayer == 1) ? m_player1Rack : m_player2Rack;
    auto& currentTextures = (m_currentPlayer == 1) ? m_tileTexturesP1 : m_tileTexturesP2;

    for (size_t i = 0; i < currentRack.size(); ++i) { // SỬA: Dùng currentRack
        char letter = currentRack[i].letter;
        if (letter != '\0') {
            SDL_Rect destRect = { RACK_X + 5 + (int)i * (RACK_TILE_SIZE + 5), RACK_Y + (RACK_HEIGHT - RACK_TILE_SIZE) / 2, RACK_TILE_SIZE, RACK_TILE_SIZE };
            auto it = currentTextures.find(letter);
            if (it != currentTextures.end()) {
                SDL_RenderCopy(m_renderer, it->second, NULL, &destRect);
            }
        }
    }
}

// Hàm renderDraggedTile (đã sửa)
void Game::renderDraggedTile() {
    if (m_isDragging) {
        auto& currentTextures = (m_currentPlayer == 1) ? m_tileTexturesP1 : m_tileTexturesP2;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        auto it = currentTextures.find(m_draggedTile.letter);
        if (it != currentTextures.end()) {
            SDL_Rect destRect = { mouseX - m_dragOffsetX, mouseY - m_dragOffsetY, RACK_TILE_SIZE, RACK_TILE_SIZE };
            SDL_RenderCopy(m_renderer, it->second, NULL, &destRect);
        }
    }
}

// Hàm initGameAssets (đã sửa)
bool Game::initGameAssets() {
    if (!m_board) m_board = new Board(m_renderer);
    if (!m_tileBag) m_tileBag = new TileBag();
    if (!m_dictionary) m_dictionary = new Dictionary("assets/dictionary.txt");

    m_player1Rack.clear();
    m_player2Rack.clear();
    m_tileBag->shuffle();
    for (int i = 0; i < 7; ++i) {
        if (!m_tileBag->isEmpty()) m_player1Rack.push_back(m_tileBag->drawTile());
        if (!m_tileBag->isEmpty()) m_player2Rack.push_back(m_tileBag->drawTile());
    }

    if (m_tileTexturesP1.empty()) {
        for (char c = 'A'; c <= 'Z'; ++c) {
            m_tileTexturesP1[c] = IMG_LoadTexture(m_renderer, ("assets/Tileblue/" + std::string(1, c) + ".png").c_str());
            if (!m_tileTexturesP1[c]) return false;
        }
        m_tileTexturesP1[' '] = IMG_LoadTexture(m_renderer, "assets/tiles/BLANK.png");
        if (!m_tileTexturesP1[' ']) return false;
    }
    if (m_tileTexturesP2.empty()) {
        for (char c = 'A'; c <= 'Z'; ++c) {
            m_tileTexturesP2[c] = IMG_LoadTexture(m_renderer, ("assets/Tilered/" + std::string(1, c) + ".png").c_str());
            if (!m_tileTexturesP2[c]) return false;
        }
        m_tileTexturesP2[' '] = IMG_LoadTexture(m_renderer, "assets/tiles/BLANK.png");
        if (!m_tileTexturesP2[' ']) return false;
    }

    if (m_gameButtonTextures.empty()) {
        m_gameButtonTextures["submit"] = IMG_LoadTexture(m_renderer, "assets/buttons/submit.png");
        m_gameButtonTextures["recall"] = IMG_LoadTexture(m_renderer, "assets/buttons/recall.png");
        m_gameButtonTextures["pass"] = IMG_LoadTexture(m_renderer, "assets/buttons/pass.png");
        if (!m_gameButtonTextures["submit"] || !m_gameButtonTextures["recall"] || !m_gameButtonTextures["pass"]) return false;

        int w, h;
        SDL_QueryTexture(m_gameButtonTextures["submit"], NULL, NULL, &w, &h);
        m_gameButtonRects["submit"] = {BOARD_X_OFFSET + BOARD_PIXEL_SIZE + 50, 200, w, h};
        SDL_QueryTexture(m_gameButtonTextures["recall"], NULL, NULL, &w, &h);
        m_gameButtonRects["recall"] = {BOARD_X_OFFSET + BOARD_PIXEL_SIZE + 50, 200 + h + 20, w, h};
        SDL_QueryTexture(m_gameButtonTextures["pass"], NULL, NULL, &w, &h);
        m_gameButtonRects["pass"] = {BOARD_X_OFFSET + BOARD_PIXEL_SIZE + 50, 200 + h + 20 + h + 20, w, h};
    }
    return true;
}

// Hàm cleanUpGameAssets (đã sửa)
void Game::cleanUpGameAssets() {
    if (m_board) { delete m_board; m_board = nullptr; }
    if (m_tileBag) { delete m_tileBag; m_tileBag = nullptr; }
    if (m_dictionary) { delete m_dictionary; m_dictionary = nullptr; }

    // SỬA: Dùng cú pháp tương thích với C++ cũ hơn và đúng tên biến
    for (auto const& pair : m_tileTexturesP1) { SDL_DestroyTexture(pair.second); }
    m_tileTexturesP1.clear();
    for (auto const& pair : m_tileTexturesP2) { SDL_DestroyTexture(pair.second); }
    m_tileTexturesP2.clear();
    for (auto const& pair : m_gameButtonTextures) { SDL_DestroyTexture(pair.second); }
    m_gameButtonTextures.clear();

    m_player1Rack.clear();
    m_player2Rack.clear();
    m_tilesOnBoardThisTurn.clear();
}

void Game::cleanUp() {
    cleanUpGameAssets();
    cleanUpMenu();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    m_renderer = nullptr;
    m_window = nullptr;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::cleanUpMenu() {
    if(m_font) { TTF_CloseFont(m_font); m_font = nullptr; }
    if(m_menuBackgroundTexture) { SDL_DestroyTexture(m_menuBackgroundTexture); m_menuBackgroundTexture = nullptr; }
    for (auto const& pair : m_menuTextures) { SDL_DestroyTexture(pair.second); } // SỬA: Dùng cú pháp cũ hơn
    m_menuTextures.clear();
}


// --- BỘ NÃO CỦA GAME (Đã sửa) ---

void Game::recallTiles() {
    auto& currentRack = (m_currentPlayer == 1) ? m_player1Rack : m_player2Rack;
    for (const auto& placedTile : m_tilesOnBoardThisTurn) {
        m_board->removeTile(placedTile.row, placedTile.col);
        currentRack.push_back(placedTile.tile);
    }
    m_tilesOnBoardThisTurn.clear();
    std::cout << "Tiles recalled!" << std::endl;
}

// Hàm handleSubmit (đã sửa)
void Game::handleSubmit() {
    if (m_tilesOnBoardThisTurn.empty()) {
        std::cout << "You haven't placed any tiles!" << std::endl;
        return;
    }

    std::vector<std::string> wordsFound;
    if (validateMove(wordsFound)) {
        int scoreThisTurn = calculateScore(m_tilesOnBoardThisTurn, wordsFound);

        auto& currentRack = (m_currentPlayer == 1) ? m_player1Rack : m_player2Rack;
        if (m_currentPlayer == 1) {
            m_player1Score += scoreThisTurn;
            std::cout << "Player 1 scored: " << scoreThisTurn << " points. Total score: " << m_player1Score << std::endl;
        } else {
            m_player2Score += scoreThisTurn;
            std::cout << "Player 2 scored: " << scoreThisTurn << " points. Total score: " << m_player2Score << std::endl;
        }

        int tilesToDraw = m_tilesOnBoardThisTurn.size();
        for(int i = 0; i < tilesToDraw; ++i) {
            if(!m_tileBag->isEmpty()) {
                currentRack.push_back(m_tileBag->drawTile());
            }
        }

        m_tilesOnBoardThisTurn.clear();
        m_isFirstMove = false;

        m_currentPlayer = (m_currentPlayer == 1) ? 2 : 1;
        std::cout << "Next turn: Player " << m_currentPlayer << std::endl; // SỬA: Thêm dấu ;
    } else {
        std::cout << "Invalid move! Recalling tiles." << std::endl;
        recallTiles();
    }
}


bool Game::validateMove(std::vector<std::string>& wordsFound) {
    const auto& tiles = m_tilesOnBoardThisTurn;
    if (tiles.empty()) return false;

    bool isHorizontal = true, isVertical = true;
    int firstRow = tiles[0].row, firstCol = tiles[0].col;
    for (size_t i = 1; i < tiles.size(); ++i) {
        if (tiles[i].row != firstRow) isHorizontal = false;
        if (tiles[i].col != firstCol) isVertical = false;
    }
    if (!isHorizontal && !isVertical) {
        std::cout << "Validation Error: Tiles must be in a single row or column." << std::endl;
        return false;
    }

    if (m_isFirstMove) {
        bool centerCovered = false;
        for (const auto& tile : tiles) if (tile.row == 7 && tile.col == 7) centerCovered = true;
        if (!centerCovered) {
            std::cout << "Validation Error: First move must cover the center star (7, 7)." << std::endl;
            return false;
        }
    }

    wordsFound.clear();
    bool connectedToOldTile = false;
    std::vector<std::string> uniqueWords;

    for (const auto& tile : tiles) {
        if (isHorizontal || tiles.size() == 1) {
            std::string word;
            int startCol = tile.col;
            while (startCol > 0 && m_board->getTileAt(tile.row, startCol - 1).letter != ' ') startCol--;
            int tempCol = startCol;
            while (tempCol < GRID_SIZE && m_board->getTileAt(tile.row, tempCol).letter != ' ') {
                word += m_board->getTileAt(tile.row, tempCol).letter;
                bool isNew = false;
                for(const auto& p : tiles) if(p.row == tile.row && p.col == tempCol) isNew = true;
                if(!isNew) connectedToOldTile = true;
                tempCol++;
            }
            if (word.length() > 1) uniqueWords.push_back(word);
        }
        if (isVertical || tiles.size() == 1) {
            std::string word;
            int startRow = tile.row;
            while (startRow > 0 && m_board->getTileAt(startRow - 1, tile.col).letter != ' ') startRow--;
            int tempRow = startRow;
            while (tempRow < GRID_SIZE && m_board->getTileAt(tempRow, tile.col).letter != ' ') {
                word += m_board->getTileAt(tempRow, tile.col).letter;
                bool isNew = false;
                for(const auto& p : tiles) if(p.row == tempRow && p.col == tile.col) isNew = true;
                if(!isNew) connectedToOldTile = true;
                tempRow++;
            }
            if (word.length() > 1) uniqueWords.push_back(word);
        }
    }

    if (!m_isFirstMove && !connectedToOldTile && tiles.size() > 0) {
        std::cout << "Validation Error: New words must connect to existing tiles." << std::endl;
        return false;
    }

    std::sort(uniqueWords.begin(), uniqueWords.end());
    uniqueWords.erase(std::unique(uniqueWords.begin(), uniqueWords.end()), uniqueWords.end());
    wordsFound = uniqueWords;

    if (wordsFound.empty()) {
        if (tiles.size() > 0 && !connectedToOldTile && m_isFirstMove) {
             // OK if it's the first move and it's just one word
        } else if (tiles.size() > 0 && connectedToOldTile) {
            // OK if it extends a word but doesn't form a new one, this case is rare
        } else {
             std::cout << "Validation Error: Must form a word of at least 2 letters." << std::endl;
             return false;
        }
    }

    for (const auto& word : wordsFound) {
        std::cout << "Found word: " << word << std::endl;
        if (!m_dictionary->isWordValid(word)) {
            std::cout << "Validation Error: '" << word << "' is not a valid word." << std::endl;
            return false;
        }
    }
    return true;
}

int Game::calculateScore(const std::vector<PlacedTile>& placedTiles, const std::vector<std::string>& words) {
    int totalScore = 0;
    for (const auto& word : words) {
        int wordScore = 0;
        int wordMultiplier = 1;
        bool isHorizontal = true;
        int startR = -1, startC = -1;

        // Find word orientation and position
        for(int r=0; r<15 && startR == -1; ++r) for(int c=0; c<15; ++c){
            if(c + word.length() <= 15) {
                std::string hWord = "";
                for(size_t i=0; i<word.length();++i) hWord += m_board->getTileAt(r,c+i).letter;
                if(hWord == word) {startR = r; startC = c; isHorizontal = true; break;}
            }
            if(r + word.length() <= 15) {
                std::string vWord = "";
                for(size_t i=0; i<word.length();++i) vWord += m_board->getTileAt(r+i,c).letter;
                if(vWord == word) {startR = r; startC = c; isHorizontal = false; break;}
            }
        }

        for (size_t i = 0; i < word.length(); ++i) {
            int r = isHorizontal ? startR : startR + i;
            int c = isHorizontal ? startC + i : startC;
            int letterScore = m_board->getTileAt(r, c).value;

            bool isNewTile = false;
            for(const auto& p : placedTiles) if(p.row == r && p.col == c) isNewTile = true;

            if (isNewTile) {
                Bonus bonus = m_board->getBonusAt(r, c);
                switch (bonus) {
                    case Bonus::DOUBLE_LETTER: letterScore *= 2; break;
                    case Bonus::TRIPLE_LETTER: letterScore *= 3; break;
                    case Bonus::DOUBLE_WORD: wordMultiplier *= 2; break;
                    case Bonus::TRIPLE_WORD: wordMultiplier *= 3; break;
                    default: break;
                }
            }
            wordScore += letterScore;
        }
        totalScore += wordScore * wordMultiplier;
    }

    if (placedTiles.size() == 7) totalScore += 50;
    return totalScore;
}
