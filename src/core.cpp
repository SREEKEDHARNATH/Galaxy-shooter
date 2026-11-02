#include "core.hpp"
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include <iostream>
#include <utility>

namespace core {
    std::pair<bool, const char*> init(){
        if (SDL_Init(SDL_INIT_EVERYTHING)){
            return {false, SDL_GetError()};
        }
        getWindow()=SDL_CreateWindow(getTitle(), getX(), getY(), getW(), getH(), SDL_WINDOW_HIDDEN);
        if (!getWindow()){
            return {false, SDL_GetError()};
        }
        getRenderer()=SDL_CreateRenderer(getWindow(), -1, 0);
        if (!getRenderer()){
            return {false, SDL_GetError()};
        }
        return {true, "All Great"};
    }
    void run(){
        auto i = init();
        if (!i.first){
            std::cerr << i.second << "\n";
            return;
        }

        SDL_Event e;
        bool running=true;

        SDL_ShowWindow(getWindow());
        while (running){
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        running=false;
                        break;
                }
            }

            SDL_RenderClear(getRenderer());
            SDL_RenderPresent(getRenderer());
            
            SDL_Delay(16);
        }
    }
    void quit(){
        if (getRenderer()){
            SDL_DestroyRenderer(getRenderer());
        }
        if (getRenderer()){
            SDL_DestroyWindow(getWindow());
        }
        SDL_Quit();
    }

    SDL_Window*& getWindow(){
        static SDL_Window* window=nullptr;
        return window;
    }
    SDL_Renderer*& getRenderer(){
        static SDL_Renderer* renderer=nullptr;
        return renderer;
    }
    
    int getX(){
        return SDL_WINDOWPOS_CENTERED;
    }
    int getY(){
        return SDL_WINDOWPOS_CENTERED;
    }
    int getW(){
        return 800;
    }
    int getH(){
        return 600;
    }
    const char* getTitle(){
        return "Galaxy shooter";
    }
}
