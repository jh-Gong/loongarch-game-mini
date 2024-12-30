#include "MH.hpp"
#include <atomic>

extern std::atomic<bool> running;

GobangPage::GobangPage(PageManager* pm) : pageManager(pm) {
    startButton = std::make_shared<Button>(412, 250, 200, 100, "Start Game", 16, 0.3, Color(215, 177, 109), Color(139, 69, 19));
    startButton->on_click([this]() {
        std::cout << "开始游戏!" << std::endl;
        pageManager->switchTo("game");
    });

    exitButton = std::make_shared<Button>(412, 400, 200, 100, "Exit", 16, 0.3, Color(215, 177, 109), Color(139, 69, 19));
    exitButton->on_click([this]() {
        std::cout << "退出游戏!" << std::endl;
        running = false;
    });

    wordsImage = std::make_shared<Image>("../resourse/image/words.bmp", Point(362, 75));
}

GobangPage::~GobangPage() = default;

void GobangPage::show() {
    if (dirtyRects.empty()) {
        drawBackground();
    }

    startButton->set_visiable(true);
    exitButton->set_visiable(true);
    
    startButton->draw();
    exitButton->draw();

    dirtyRects.clear();
}

void GobangPage::hide() {
    startButton->set_visiable(false);
    exitButton->set_visiable(false);
}

void GobangPage::update() {
    // 触摸事件
    if (startButton->get_is_touched()){
        startButton->set_box_color(Color(193, 154, 91));
    }else{
        startButton->set_box_color(Color(215, 177, 109));
    }
    
    if (exitButton->get_is_touched())
    {
        exitButton->set_box_color(Color(193, 154, 91));
    }else{
        exitButton->set_box_color(Color(215, 177, 109));
    }
    
    // 点击事件
    if (startButton->get_is_clicked()){
        startButton->set_is_clicked(false);
        startButton->click();
        return;
    }else if (exitButton->get_is_clicked())
    {
        exitButton->set_is_clicked(false);
        exitButton->click();
        return;
    }

    dirtyRects.emplace_back(startButton->get_position(), startButton->get_width(), startButton->get_height());
    dirtyRects.emplace_back(exitButton->get_position(), exitButton->get_width(), exitButton->get_height());

}

bool GobangPage::updata_mouse_position(int x, int y, int p){
    bool initTouch_flag = startButton->get_is_touched() || exitButton->get_is_touched();

    bool isStartButtonInside = startButton->is_inside(x, y);
    if (isStartButtonInside && p == 0 && startButton->get_is_touched()){
        startButton->set_is_clicked(true);
    }
    startButton->set_is_touched(isStartButtonInside && p > 0);

    bool isExitButtonInside = exitButton->is_inside(x, y);
    if (isExitButtonInside && p == 0 && exitButton->get_is_touched()){
        exitButton->set_is_clicked(true);
    }
    exitButton->set_is_touched(isExitButtonInside && p > 0);


    return isStartButtonInside || isExitButtonInside || (initTouch_flag && p > 0);
}

void GobangPage::drawBackground(){
    Rectangle(0, 0, 1024, 600, true, 0., Color(247, 215, 148)).draw();
    wordsImage->draw();
}