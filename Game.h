#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector> // Thêm include này cho std::vector
#include <map>
#include "Board.h"
#include "TileBag.h"
#include "Dictionary.h"
// Định nghĩa các trạng thái của game
enum class GameState {
    MAIN_MENU,
    PLAYING_PVP,
    PLAYING_AI,
    GUIDE,
    EXIT
};
struct PlacedTile {
    Tile tile;
    int row, col;
};
class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();


private:
    // === Các hàm điều khiển chính ===
    void processEvents();
    void update();
    void render();
    void cleanUp();

    // === Các hàm dành riêng cho MENU ===
    bool initMenu();
    void handleMenuEvents(SDL_Event& e);
    void renderMenu();
    void cleanUpMenu();

    // === Các hàm dành riêng cho LÚC CHƠI GAME ===
    bool initGameAssets();
    void handleGameEvents(SDL_Event& e);
    void renderGame();
    void renderPlayerRack();
    void renderDraggedTile();
    void cleanUpGameAssets();

    void handleSubmit();
    void recallTiles();
    bool validateMove(std::vector<std::string>& wordsFound);
    int calculateScore(const std::vector<PlacedTile>& placedTiles, const std::vector<std::string>& words);
    // === Các biến trạng thái và hệ thống ===
    bool m_isRunning;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    GameState m_currentState;

    // === Con trỏ tới các đối tượng game ===
    Board* m_board;
    TileBag* m_tileBag;
    Dictionary* m_dictionary;
    int m_currentPlayer;
    // === Dữ liệu người chơi và trạng thái kéo thả ===
    std::vector<Tile> m_player1Rack;
    std::vector<Tile> m_player2Rack;
    std::vector<PlacedTile> m_tilesOnBoardThisTurn; // Danh sách các ô đã đặt trong lượt

    int m_player1Score;// <-- THÊM BIẾN ĐIỂM SỐ
    int m_player2Score;
    bool m_isFirstMove; // <-- THÊM BIẾN KIỂM TRA LƯỢT ĐẦU

    bool m_isDragging;
    Tile m_draggedTile;
    int m_draggedTileOriginalIndex;
    int m_dragOffsetX, m_dragOffsetY;

    // === Tài nguyên chung cho game ===
    std::map<char, SDL_Texture*> m_tileTexturesP1;
    std::map<char, SDL_Texture*> m_tileTexturesP2;

    std::map<std::string, SDL_Texture*> m_gameButtonTextures;
    std::map<std::string, SDL_Rect> m_gameButtonRects;
    // === Tài nguyên riêng cho MENU ===
    TTF_Font* m_font;
    SDL_Texture* m_menuBackgroundTexture;
    SDL_Color m_textColor;
    SDL_Color m_highlightColor;
    std::map<std::string, SDL_Texture*> m_menuTextures;
    std::map<std::string, SDL_Rect> m_menuRects;
};

#endif // GAME_H
