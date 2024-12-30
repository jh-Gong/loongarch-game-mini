#include "ui.hpp"
#include "MH.hpp"

#include <chrono>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>


std::atomic<bool> running(true);
std::mutex coordMutex;
std::condition_variable coordCondVar;
bool mouse_data_ready = false;
int coordX = 0, coordY = 0, coordPressure = 0;

void readCoordinates(struct tsdev *ts) {
    struct ts_sample samp;
    // 设置为非阻塞模式
    int flags = fcntl(ts_fd(ts), F_GETFL, 0);
    fcntl(ts_fd(ts), F_SETFL, flags | O_NONBLOCK);

    // 上次采样的压力值
    int lastPressure = -1;
    // 上次采样的时间点
    auto lastSampleTime = std::chrono::steady_clock::now();
    // 采样时间间隔 (50ms)
    const auto SAMPLE_INTERVAL = std::chrono::milliseconds(20);

    while (running) {
        int ret = ts_read(ts, &samp, 1);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } else {
                perror("ts_read");
                running = false;
                return;
            }
        } else if (ret == 1) {
            // printf("x: %d, y: %d, pressure: %d\n", samp.x, samp.y, samp.pressure);
            auto now = std::chrono::steady_clock::now();
            // 如果是松开事件，立即处理
            if (samp.pressure == 0) {
                // 1. 发送压力值为 255 的事件
                {
                    std::unique_lock<std::mutex> lock(coordMutex);
                    coordX = samp.x;
                    coordY = samp.y;
                    coordPressure = 255; // 模拟按下结束
                    mouse_data_ready = true;
                    coordCondVar.notify_one();
                }

                // 2. 发送压力值为 0 的事件
                {
                    std::unique_lock<std::mutex> lock(coordMutex);
                    coordX = samp.x;
                    coordY = samp.y;
                    coordPressure = 0; // 真正的松开事件
                    mouse_data_ready = true;
                    coordCondVar.notify_one();
                }

                lastPressure = samp.pressure;
                lastSampleTime = now;
            } else if (lastPressure > 0) {
                // 如果是按下状态，并且距离上次采样时间超过了阈值，则处理
                if (now - lastSampleTime >= SAMPLE_INTERVAL) {
                    std::unique_lock<std::mutex> lock(coordMutex);
                    coordX = samp.x;
                    coordY = samp.y;
                    coordPressure = samp.pressure;
                    mouse_data_ready = true;
                    coordCondVar.notify_one();
                    lastPressure = samp.pressure;
                    lastSampleTime = now;
                }
            } else {
                // 第一次按下
                std::unique_lock<std::mutex> lock(coordMutex);
                coordX = samp.x;
                coordY = samp.y;
                coordPressure = samp.pressure;
                mouse_data_ready = true;
                coordCondVar.notify_one();
                lastPressure = samp.pressure;
                lastSampleTime = now;
            }
        }
    }
}

int main(){
    
    bitmap_font_ptr = std::make_unique<BitmapFont>();
    
    struct tsdev *ts;

    PageManager page_manager;
    page_manager.addPage("gobang", std::make_shared<GobangPage>(&page_manager));
    page_manager.addPage("game", std::make_shared<GamePage>(&page_manager));
    page_manager.switchTo("gobang");

    ts = ts_setup(NULL, 0);
    if (!ts) {
        perror("ts_setup");
        exit(1);
    }
    
    std::thread coordThread(readCoordinates, ts);
    
    while (running) {
        
        #ifdef USE_DEBUG
        auto start = std::chrono::high_resolution_clock::now(); 
        #endif
        
        {
            std::unique_lock<std::mutex> lock(coordMutex);
            coordCondVar.wait(lock, []{ return mouse_data_ready || !running; }); 

            if (!running){
                break;
            }

            page_manager.updateCurrentPage(coordX, coordY, coordPressure);
            mouse_data_ready = false;
        }
        #ifdef USE_DEBUG
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Elapsed time: " << duration.count()  << " ms" << std::endl;
        #endif
    }

    running = false;
    coordCondVar.notify_all();
    coordThread.join();
    ts_close(ts);
    return 0;

}