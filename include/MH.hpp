#ifndef __MH_HPP__
#define __MH_HPP__

#include "tslib/tslib.h"
#include "ui.hpp"

class GobangPage : public Page {
public:
    GobangPage(PageManager* pm);
    ~GobangPage() override;

    void show() override;
    void hide() override;
    void update() override;
    bool updata_mouse_position(int x, int y, int p) override;

private:
    std::shared_ptr<Button> startButton;
    std::shared_ptr<Button> exitButton;
    std::shared_ptr<Image> wordsImage;
    PageManager* pageManager;

    std::vector<std::tuple<Point, int, int>> dirtyRects;

private:
    void drawBackground();
};

class GamePage : public Page {
public:
    GamePage(PageManager* pm);
    ~GamePage() override;

    void show() override;
    void hide() override;
    void update() override;
    bool updata_mouse_position(int x, int y, int p) override;

private:
    std::shared_ptr<Button> resetButton;
    std::shared_ptr<Button> backButton;
    std::shared_ptr<Image> boardImage;
    std::shared_ptr<Image> wordsImage;
    std::shared_ptr<String> gameStateWords;
    PageManager* pageManager;

    std::vector<std::tuple<Point, int, int>> dirtyRects;

private:
    void drawBoard();
    void drawBackground();
    void resetGame();
    bool checkDraw();
    bool checkDirection(int x, int y, int dx, int dy);
    bool checkWin(int x, int y);
    void drawGameState();
    void drawChessPieces();
};

#endif
