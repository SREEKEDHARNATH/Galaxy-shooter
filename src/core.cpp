#include "core.hpp"
#include "SDL.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_timer.h"
#include "SDL_ttf.h"
#include "SDL_video.h"
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include "api.hpp"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "listeners.hpp"
#include "SDL_mixer.h"
#include "physics.hpp"

class asteroid{
    public:
        int speed=4;
        float size=1;
        core::api::image im;
        int x=0,y=0;
        asteroid(SDL_Texture* tex, int x1=0, int y1=0, float sz=1, int spd=4) : im("",false), x(x1), y(y1), size(sz), speed(spd){ 
            im.set(tex);
            im.dstrect.w*=size;
            im.dstrect.h*=size;
        }
        bool update(int x1=0, int y1=0, float sz=1, int spd=4){
            y+=speed;
            im.dstrect.x=x;
            im.dstrect.y=y;
            if (outofbounds()) {
                reset(x1, y1, sz, spd);
                return true;
            }
            return false;
        }
        void draw(){
            im.draw();
        }
        void reset(int x1=0, int y1=0, float sz=1, int spd=4){
            x=x1;
            y=y1;
            size=sz;
            speed=spd;
            im.recallibrate();
            im.dstrect.w*=size;
            im.dstrect.h*=size;
        }
        bool outofbounds(){
            return (y+im.dstrect.w)>core::getH();
        }
};
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
        api::text scoretext(f,"Score:- ");
        int score=0;
        api::text sc(f, "0");
        sc.i.dstrect.x=scoretext.i.dstrect.w+10;
        api::text pressspace(f, "Press Space To start");

        api::image bg("assets/images/map.png", true, false);
        api::image ship("assets/images/ship.png");
        api::image bullet("assets/images/bullet.png");
        physics::world world({0,5});
        physics::body body(world.get(), {0,400/32.0f}, b2_dynamicBody);
        body.attachBox(ship.dstrect.w/32.0f, ship.dstrect.h/32.0f);

        std::vector<std::pair<int, int>> bullets(10);
        float start,dt=0;
        float bulletintervelmax=9;
        float bulletintervel=bulletintervelmax;
        float bulletspeed=5;

        api::image aster("assets/images/asteroid.png");
        api::image asterhit("assets/images/asteroid-hit.png");
        api::image asterexplode("assets/images/asteroid-explode.png");
        
        asteroid ast(aster.get().get(), 400);
        bool hit=false,explode=false;
        int hitcount=0;
        const float hitstaytimemax=10;
        float hitstaytime=hitstaytimemax;
        const float explodestaytimemax=8;
        float explodestaytime=explodestaytimemax;
        int astspeed=3;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(0,(getW()/100));

        int maxforupgrade=3;
        bool play=false;
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
            if (keylistener::isPressed(SDLK_SPACE)){
                play=true;
            }
            if (play){
                if (keylistener::isDown(SDLK_RIGHT) || keylistener::isDown(SDLK_d)){
                    b2Body_ApplyForceToCenter(body.get(), {40,0}, false);
                }
                else if (mouselistener::isDown(SDL_BUTTON_RIGHT)){
                    b2Body_ApplyForceToCenter(body.get(), {40,0}, false);
                }
    
                if (keylistener::isDown(SDLK_LEFT) || keylistener::isDown(SDLK_a)){
                    b2Body_ApplyForceToCenter(body.get(), {-40,0}, false);
                }
                else if (mouselistener::isDown(SDL_BUTTON_LEFT)){
                    b2Body_ApplyForceToCenter(body.get(), {-40,0}, false);
                }
    
                if (keylistener::isDown(SDLK_UP) || keylistener::isDown(SDLK_w)){
                    b2Body_ApplyForceToCenter(body.get(), {0,-60}, false);
                }
                else if (mouselistener::getScrly()<0){
                    b2Body_ApplyForceToCenter(body.get(), {0,-40}, false);
                }
    
                if (keylistener::isDown(SDLK_DOWN) || keylistener::isDown(SDLK_s)){
                    b2Body_ApplyForceToCenter(body.get(), {-0,20}, false);
                }
                else if (mouselistener::getScrly()>0){
                    b2Body_ApplyForceToCenter(body.get(), {-0,5}, false);
                }
                
                ast.speed=astspeed;
                if (score >= maxforupgrade){
                    bulletintervelmax-=1;
                    bulletspeed+=3;
                    maxforupgrade*=4;
                    astspeed++;
                }
                if (score==100){
                    running=false;
                    std::cout << "nice dedication\n";
                }
                world.simulate();
                auto pos = b2Body_GetPosition(body.get());
    
                ship.dstrect.x=pos.x*32;
                ship.dstrect.y=pos.y*32;
    
                bullet.dstrect.x=(pos.x*32)+(ship.dstrect.w/2.0f);
                bullet.dstrect.y=pos.y*32;
                bulletintervel-=dt;
                if (bulletintervel<0){
                    bulletintervel=bulletintervelmax;
                    bullets.push_back({bullet.dstrect.x, bullet.dstrect.y});
                    if (score>=10){
                        bullets.push_back({bullet.dstrect.x-((ship.dstrect.w/2.0f)-5), bullet.dstrect.y});
                    }
                    if (score>=50){
                        bullets.push_back({bullet.dstrect.x+(ship.dstrect.w/2.0f)-10, bullet.dstrect.y});
                    }
                }
    
                if ((ship.dstrect.x+32)< 0 || (ship.dstrect.x+ship.dstrect.w-32)>getW()){
                    running=false;
                }
                if ((ship.dstrect.y+32)< 0 || (ship.dstrect.y+ship.dstrect.h-32)>getH()){
                    running=false;
                }
                for (int i=0;i<bullets.size();i++){
                    SDL_Rect r={bullets[i].first, bullets[i].second, bullet.dstrect.w, bullet.dstrect.h};
                    if (ast.y > ast.im.dstrect.h){
                        if (SDL_HasIntersection(&r, &ast.im.dstrect)){
                            bullets[i].second-=(ast.im.dstrect.h+10);
                            hit=true;
                            i=bullets.size();
                            hitcount++;
                        }
                    }
                }
                if (!explode){
                    ast.update(distrib(gen)*100);
                }
                if ((hitcount-6)>=ast.size){
                    explode=true;
                }
                if (SDL_HasIntersection(&ast.im.dstrect, &ship.dstrect)){
                    running=false;
                }
            }
            SDL_RenderClear(getRenderer());
            if (play){
                bg.draw();
                if (explode){
                    asterexplode.dstrect=ast.im.dstrect;
                    asterexplode.draw();
                }
                else if (hit){
                    asterhit.dstrect=ast.im.dstrect;
                    asterhit.draw();
                }
                else {
                    ast.draw();
                }
                if (hit){
                    hitstaytime-=dt;
                    if (hitstaytime<0){
                        hit=false;
                        hitstaytime=hitstaytimemax;
                    }
                }
                else {
                    hit=false;
                }
                if (explode){
                    explodestaytime-=dt;
                    if (explodestaytime<0){
                        explode=false;
                        explodestaytime=explodestaytimemax;
                        ast.reset(distrib(gen)*100);
                        hitcount=0;
                        score++;
                    }
                }
                else {
                    explode=false;
                }
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
                scoretext.draw();
                sc.draw();
            }
            else {
                pressspace.draw();
            }
            SDL_RenderPresent(getRenderer());
            if (play){
                if (std::stoi(sc.get()) != score){
                    sc.setText(f, std::to_string(score).c_str());
                    sc.i.dstrect.x=scoretext.i.dstrect.w+10;
                }
            }
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
