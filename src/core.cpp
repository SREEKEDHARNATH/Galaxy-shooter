#include "core.hpp"
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_keycode.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include <iostream>
#include <utility>
#include "api.hpp"
#include "listeners.hpp"
#include "SDL_mixer.h"
#include "physics.hpp"

namespace core {
    std::pair<bool, const char*> init(){
        int img_flags = IMG_INIT_PNG || IMG_INIT_JPG;
        int sound_flags = MIX_INIT_MP3;
        if (SDL_Init(SDL_INIT_EVERYTHING)){
            return {false, SDL_GetError()};
        }
        if ((IMG_Init(img_flags) & img_flags) != img_flags){
            return {false, IMG_GetError()};
        }
        if (TTF_Init()){
            return {false,TTF_GetError()};
        }
        if ((Mix_Init(sound_flags) & sound_flags) != sound_flags){
            return {false,Mix_GetError()};
        }
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)){
            return {false,Mix_GetError()};
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
        api::image img("", false);
        img.set(255, 0,0,32,32);
        api::font f("assets/fonts/font.ttf");
        api::text t(f, "Hello guys", {0,255,0,255});
        physics::world world;

        SDL_ShowWindow(getWindow());
        while (running){
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        running=false;
                        break;
                    case SDL_KEYUP:
                    case SDL_KEYDOWN:
                        keylistener::reg(e.key);
                        break;
                    case SDL_MOUSEMOTION:
                        mouselistener::reg(e.motion);
                        break;
                    case SDL_MOUSEWHEEL:
                        mouselistener::reg(e.wheel);
                        break;
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEBUTTONDOWN:
                        mouselistener::reg(e.button);
                        break;
                }
            }
            if (keylistener::isPressed(SDLK_ESCAPE)){
                running=false;
            }
            world.simulate();
            SDL_RenderClear(getRenderer());
            img.draw();
            t.draw();
            SDL_RenderPresent(getRenderer());
            
            keylistener::update();
            mouselistener::update();
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
        Mix_CloseAudio();
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
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
