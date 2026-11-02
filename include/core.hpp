#pragma once
#include "SDL_render.h"

namespace core {
    void run();
    void quit();

    SDL_Window*& getWindow();
    SDL_Renderer*& getRenderer();
    
    int getX();
    int getY();
    int getW();
    int getH();
    const char* getTitle();
}