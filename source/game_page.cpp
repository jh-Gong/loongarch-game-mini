#include "MH.hpp"


// 棋盘格大小
#define GRID_SIZE 50 
// 棋盘边距
#define BOARD_MARGIN_X 473
#define BOARD_MARGIN_Y 50
// 棋子半径
#define CHESS_RADIUS (GRID_SIZE / 2 - 5)
// 棋盘交叉点数量
#define CROSS_POINTS 11
// 胜利所需连续棋子数
#define WIN_COUNT 5

enum class ChessState {
    EMPTY,
    BLACK,
    WHITE
};

enum class GameState {
    PLAYING,
    BLACK_WON,
    WHITE_WON,
    DRAW
};

// 游戏棋盘
ChessState board[CROSS_POINTS][CROSS_POINTS] = {ChessState::EMPTY};
// 当前落子方
ChessState currentPlayer = ChessState::BLACK;
// 游戏状态
GameState gameState = GameState::PLAYING;

GamePage::GamePage(PageManager* pm) : pageManager(pm) {
    resetButton = std::make_shared<Button>(100, 250, 200, 100, "Restart", 16, 0.2, Color(215, 177, 109), Color(139, 69, 19));
    resetButton->on_click([this]() {
        std::cout << "重新开始游戏!" << std::endl;
        resetGame();
    });

    backButton = std::make_shared<Button>(100, 400, 200, 100, "Back", 16, 0.2, Color(215, 177, 109), Color(139, 69, 19));
    backButton->on_click([this]() {
        std::cout << "返回主菜单!" << std::endl;
        pageManager->switchTo("gobang"); 
    });

    boardImage = std::make_shared<Image>("../resourse/image/board.bmp", Point(423, 0));
    wordsImage = std::make_shared<Image>("../resourse/image/words.bmp", Point(50, 50));
    gameStateWords = std::make_shared<String>("", Point(100, 200), 16, BLACK);
}

GamePage::~GamePage() = default;

void GamePage::show() {
    resetButton->set_visiable(true);
    backButton->set_visiable(true);
    // 绘制背景
    drawBackground();

    // 绘制棋盘和棋子
    drawBoard();
    drawChessPieces();

    // 显示游戏状态
    drawGameState();

    resetButton->draw();
    backButton->draw();
}

void GamePage::hide() {
    resetButton->set_visiable(false);
    backButton->set_visiable(false);
    Rectangle(Point(0, 0), 1024, 600, true, 0., BLACK).draw();
}

void GamePage::update() {
    // 处理鼠标事件
    if (resetButton->get_is_clicked()){
        resetButton->set_is_clicked(false);
        resetButton->click();
        return;
    }else if (backButton->get_is_clicked())
    {
        backButton->set_is_clicked(false);
        backButton->click();
        return;
    }
}

bool GamePage::updata_mouse_position(int x, int y, int p) {
    bool isResetButtonInside = resetButton->is_inside(x, y);
    if (isResetButtonInside && p == 0 && resetButton->get_is_touched()){
        resetButton->set_is_clicked(true);
    }
    resetButton->set_is_touched(isResetButtonInside && p > 0);

    bool isBackButtonInside = backButton->is_inside(x, y);
    if (isBackButtonInside && p == 0 && backButton->get_is_touched()){
        backButton->set_is_clicked(true);
    }
    backButton->set_is_touched(isBackButtonInside && p > 0);

    // 如果游戏正在进行中，并且鼠标按下
    bool update_flag = gameState == GameState::PLAYING && p == 0;
    if (update_flag) {
        // 将鼠标坐标转换为棋盘坐标，并四舍五入到最近的交叉点
        int boardX = round((double)(x - BOARD_MARGIN_X) / GRID_SIZE);
        int boardY = round((double)(y - BOARD_MARGIN_Y) / GRID_SIZE);

        // 检查是否在棋盘范围内
        if (boardX >= 0 && boardX < CROSS_POINTS && boardY >= 0 && boardY < CROSS_POINTS) {
            // 检查该位置是否为空
            if (board[boardY][boardX] == ChessState::EMPTY) {
                // 放置棋子
                board[boardY][boardX] = currentPlayer;

                // 检查游戏是否结束
                if (checkWin(boardX, boardY)) {
                    gameState = (currentPlayer == ChessState::BLACK) ? GameState::BLACK_WON : GameState::WHITE_WON;
                } else if (checkDraw()) {
                    gameState = GameState::DRAW;
                } else {
                    // 切换玩家
                    currentPlayer = (currentPlayer == ChessState::BLACK) ? ChessState::WHITE : ChessState::BLACK;
                }
            }
        }
    }

    return isResetButtonInside || isBackButtonInside || update_flag;
}

void GamePage::drawBoard(){
    boardImage->draw();
    Rectangle(Point(BOARD_MARGIN_X, BOARD_MARGIN_Y), 500, 500, false, 0., BLACK).draw();
    // 绘制棋盘内部的交叉线
    for (int i = 0; i < CROSS_POINTS; i++) {
        // 水平线
        Point startH(BOARD_MARGIN_X, BOARD_MARGIN_Y + i * GRID_SIZE);
        Point endH(BOARD_MARGIN_X + (CROSS_POINTS - 1) * GRID_SIZE, BOARD_MARGIN_Y + i * GRID_SIZE);
        Line(startH, endH, BLACK).draw();

        // 垂直线
        Point startV(BOARD_MARGIN_X + i * GRID_SIZE, BOARD_MARGIN_Y);
        Point endV(BOARD_MARGIN_X + i * GRID_SIZE, BOARD_MARGIN_Y + (CROSS_POINTS - 1) * GRID_SIZE);
        Line(startV, endV, BLACK).draw();
    }
}

void GamePage::drawBackground(){
    Rectangle(Point(0, 0), 424, 600, true, 0., Color(247, 215, 148)).draw();
    wordsImage->draw();
}

void GamePage::drawChessPieces() {
    // 绘制棋子
    for (int y = 0; y < CROSS_POINTS; y++) {
        for (int x = 0; x < CROSS_POINTS; x++) {
            if (board[y][x] != ChessState::EMPTY) {
                int screenX = BOARD_MARGIN_X + x * GRID_SIZE;
                int screenY = BOARD_MARGIN_Y + y * GRID_SIZE;
                if (board[y][x] == ChessState::BLACK) {
                    Circle(Point(screenX, screenY), CHESS_RADIUS, true, Color(0, 0, 0)).draw();
                } else {
                    Circle(Point(screenX, screenY), CHESS_RADIUS, true, Color(255, 255, 255)).draw();
                }
            }
        }
    }
}

void GamePage::drawGameState() {
    // 显示游戏状态
    switch (gameState) {
        case GameState::PLAYING:
            if (currentPlayer == ChessState::BLACK) {
                gameStateWords->set_new_text("Black's Turn").set_new_color(BLACK).draw();
            } else {
                gameStateWords->set_new_text("White's Turn").set_new_color(WHITE).draw();
            }
            break;
        case GameState::BLACK_WON:
            gameStateWords->set_new_text("Black Won!").set_new_color(BLACK).draw();
            break;
        case GameState::WHITE_WON:
            gameStateWords->set_new_text("White Won!").set_new_color(WHITE).draw();
            break;
        case GameState::DRAW:
            gameStateWords->set_new_text("Draw!").set_new_color(BLACK).draw();
            break;
    }
}

bool GamePage::checkWin(int x, int y) {
    // 检查指定位置的棋子是否赢得了游戏
    return checkDirection(x, y, 1, 0) ||  // 水平
           checkDirection(x, y, 0, 1) ||  // 垂直
           checkDirection(x, y, 1, 1) ||  // 右下斜
           checkDirection(x, y, 1, -1);   // 右上斜
}

bool GamePage::checkDirection(int x, int y, int dx, int dy) {
    // 检查指定方向上是否有连续的 WIN_COUNT 个棋子
    int count = 1;
    ChessState player = board[y][x];

    // 向正方向检查
    for (int i = 1; i < WIN_COUNT; i++) {
        int nx = x + i * dx;
        int ny = y + i * dy;
        if (nx >= 0 && nx < CROSS_POINTS && ny >= 0 && ny < CROSS_POINTS && board[ny][nx] == player) {
            count++;
        } else {
            break;
        }
    }

    // 向反方向检查
    for (int i = 1; i < WIN_COUNT; i++) {
        int nx = x - i * dx;
        int ny = y - i * dy;
        if (nx >= 0 && nx < CROSS_POINTS && ny >= 0 && ny < CROSS_POINTS && board[ny][nx] == player) {
            count++;
        } else {
            break;
        }
    }

    return count >= WIN_COUNT;
}

bool GamePage::checkDraw() {
    // 检查是否平局
    for (int y = 0; y < CROSS_POINTS; y++) {
        for (int x = 0; x < CROSS_POINTS; x++) {
            if (board[y][x] == ChessState::EMPTY) {
                return false; // 还有空位，游戏未结束
            }
        }
    }
    return true; // 没有空位，平局
}

void GamePage::resetGame() {
    // 重置游戏状态
    for (int y = 0; y < CROSS_POINTS; y++) {
        for (int x = 0; x < CROSS_POINTS; x++) {
            board[y][x] = ChessState::EMPTY;
        }
    }
    currentPlayer = ChessState::BLACK;
    gameState = GameState::PLAYING;
}