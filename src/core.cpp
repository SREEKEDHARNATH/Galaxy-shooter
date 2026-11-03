#include "core.hpp"
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_keycode.h"
#include "SDL_render.h"
#include "SDL_timer.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include <iostream>
#include <utility>
#include <vector>
#include "api.hpp"
#include "box2d/box2d.h"
#include "box2d/types.h"
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
        api::font f("assets/fonts/font.ttf");
        api::image bg("assets/images/map.png", true, false);
        api::image ship("assets/images/ship.png");
        api::image bullet("assets/images/bullet.png");
        physics::world world({0,5});
        physics::body body(world.get(), {0,0}, b2_dynamicBody);
        body.attachBox(ship.dstrect.w/32.0f, ship.dstrect.h/32.0f);

        std::vector<std::pair<int, int>> bullets(10);
        float start,dt=0;
        constexpr float bulletintervelmax=9000;
        float bulletintervel=bulletintervelmax;
        float bulletspeed=5;

        SDL_ShowWindow(getWindow());
        while (running){
            start=SDL_GetTicks64();
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
            if (keylistener::isDown(SDLK_RIGHT)){
                b2Body_ApplyForceToCenter(body.get(), {40,0}, false);
            }
            if (keylistener::isDown(SDLK_LEFT)){
                b2Body_ApplyForceToCenter(body.get(), {-40,0}, false);
            }
            if (keylistener::isDown(SDLK_UP)){
                b2Body_ApplyForceToCenter(body.get(), {0,-60}, false);
            }
            if (keylistener::isDown(SDLK_DOWN)){
                b2Body_ApplyForceToCenter(body.get(), {-0,20}, false);
            }
            
            world.simulate();
            auto pos = b2Body_GetPosition(body.get());

            ship.dstrect.x=pos.x*32;
            ship.dstrect.y=pos.y*32;

            bullet.dstrect.x=(pos.x*32)+(ship.dstrect.w/2.0f);
            bullet.dstrect.y=pos.y*32;
            bulletintervel-=(dt*1000);
            if (bulletintervel<0){
                bulletintervel=bulletintervelmax;
                bullets.push_back({bullet.dstrect.x, bullet.dstrect.y});
            }

            if ((ship.dstrect.x+32)< 0 || (ship.dstrect.x+ship.dstrect.w-32)>getW()){
                running=false;
            }
            if ((ship.dstrect.y+32)< 0 || (ship.dstrect.y+ship.dstrect.h-32)>getH()){
                running=false;
            }

            SDL_RenderClear(getRenderer());
            bg.draw();
            ship.draw();

            for (int i=0;i<bullets.size();i++){
                bullets[i].second-=bulletspeed;
                bullet.dstrect.x=bullets[i].first;
                bullet.dstrect.y=bullets[i].second;
                
                bullet.draw();
                if (bullets[i].second < 0){
                    bullets.erase(bullets.begin()+i);
                    i--;
                }
            }
            SDL_RenderPresent(getRenderer());
            
            keylistener::update();
            mouselistener::update();

            dt=SDL_GetTicks64()-start;
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
