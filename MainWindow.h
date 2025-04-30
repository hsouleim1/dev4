
#ifndef TESTINSTRUCT_MAINWINDOW_H
#define TESTINSTRUCT_MAINWINDOW_H

#include <SDL3/SDL.h>
class MainWindow {
public :
    void init();
    void run();
private:
    void draw();
    SDL_Window* window { nullptr };
    SDL_Renderer* renderer { nullptr };
};

#endif //TESTINSTRUCT_MAINWINDOW_H
