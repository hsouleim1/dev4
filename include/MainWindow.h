#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <SDL3/SDL.h>
#include <chrono>

class MainWindow {
public:
    void init();
    void run();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Timer de note
    std::chrono::steady_clock::time_point noteStart;
    bool noteActive = false;
};

#endif
