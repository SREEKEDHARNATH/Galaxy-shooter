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
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
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
    void run(int argc, char** argv){
        auto i = init();
        if (!i.first){
            std::cerr << i.second << "\n";
            return;
        }
        bool cheat=false,mute=false,file=true,mousecontrols=true,keycontrols=true,verbose=false;
        if (argc>1){
            for (int i=1;i<argc;i++){
                if (std::string(argv[i])=="-discard-arguements"){
                    cheat=false;
                    mute=false;
                    file=true;
                    mousecontrols=true;
                    keycontrols=true;
                    verbose=false;
                    i=argc;
                }
                else if (std::string(argv[i])=="-cheat=on"){
                    cheat=true;
                    file=false;
                    if (verbose){
                        std::cout << "cheat is enabled file saving is disabled :(\n";
                    }
                }
                else if (std::string(argv[i])=="-cheat=off"){
                    cheat=false;
                    file=true;
                    if (verbose){
                        std::cout << "cheat is disabled file saving is enabled :)\n";
                    }
                }                
                else if (std::string(argv[i])=="-mute=on"){
                    mute=true;
                    if (verbose){
                        std::cout << "muting..... :(\n";
                    }
                }
                else if (std::string(argv[i])=="-mute=off"){
                    mute=false;
                    if (verbose){
                        std::cout << "unmuting..... :)\n";
                    }
                }
                else if (std::string(argv[i])=="-file=on"){
                    file=true;
                    if (verbose){
                        std::cout << "file saving on :)\n";
                    }
                }
                else if (std::string(argv[i])=="-file=off"){
                    file=false;
                    if (verbose){
                        std::cout << "file saving off :(\n";
                    }
                }
                else if (std::string(argv[i])=="-key=on"){
                    keycontrols=true;
                    if (verbose){
                        std::cout << "keycontrols! on :)\n";
                    }
                }
                else if (std::string(argv[i])=="-key=off"){
                    keycontrols=false;
                    if (verbose){
                        std::cout << "keycontrols! off :(\n";
                    }
                }
                else if (std::string(argv[i])=="-mouse=on"){
                    mousecontrols=true;
                    if (verbose){
                        std::cout << "mousecontrols! on :)\n";
                    }
                }
                else if (std::string(argv[i])=="-mouse=off"){
                    mousecontrols=false;
                    if (verbose){
                        std::cout << "mousecontrols! on :(\n";
                    }
                }
                else if (std::string(argv[i])=="-verbose=on"){
                    verbose=true;
                    if (verbose){
                        std::cout << "you made me on :)\n";
                    }
                }
                else if (std::string(argv[i])=="-verbose=off"){
                    verbose=false;
                    if (verbose){
                        std::cout << "noooooooo on :(\n";
                    }
                }
            }
        }
        if (cheat){
            std::string s;
            std::cout << "Why did you enable cheats:- ";
            std::cin >> s;
            std::cout << "You could have turned this off and win like a warrior:- ";
            std::cin >> s;
            std::cout << "You know right you could just restart:- ";
            std::cin >> s;
            std::cout << "Are you sure (y/n):- ";
            std::cin >> s; 
            if (s=="n" || s=="N"){
                std::cout << "Great real warriors dont use cheats\n";
                return;
            }
            else {
                std::cout << ":(\n";
            }
        }
        if (!mousecontrols && !keycontrols){
            std::cout << "Warning:- All controls are disabled\n";
        }
        SDL_Event e;
        bool running=true;
        api::font f("assets/fonts/font.ttf");
        api::text scoretext(f,"Score:- ");
        int score=0;
        api::text sc(f, "0");
        sc.i.dstrect.x=scoretext.i.dstrect.w+10;

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
        int astspeed=cheat ? 1 : 3;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> distrib(0,(getW()/100));

        int maxforupgrade=cheat ? 1 : 3;
        bool play=false;

        api::image unfilledplaybutton("assets/images/unfilled-play-button.png");
        unfilledplaybutton.dstrect.x=(getW()/2.0f)-120;
        unfilledplaybutton.dstrect.y=(getH()/2.0f)-60;
        api::image filledplaybutton("assets/images/filled-play-button.png");
        filledplaybutton.dstrect=unfilledplaybutton.dstrect;

        api::image unfilledquitbutton("assets/images/unfilled-quit-button.png");
        unfilledquitbutton.dstrect=unfilledplaybutton.dstrect;
        unfilledquitbutton.dstrect.y+=90;
        api::image filledquitbutton("assets/images/filled-quit-button.png");
        filledquitbutton.dstrect=unfilledquitbutton.dstrect;

        SDL_Rect mouse={0,0,1,1};
        auto starttime = std::chrono::steady_clock::now();
        bool escape=false;

        api::image unfilledyes("assets/images/unfilled-yes.png");
        api::image unfilledno("assets/images/unfilled-no.png");
        api::image unfilledframe("assets/images/unfilled-frame.png");
        api::image filledyes("assets/images/filled-yes.png");
        api::image filledno("assets/images/filled-no.png");
        api::image filledframe("assets/images/filled-frame.png");
        
        unfilledframe.dstrect.x=(getW()/2.0f)-180;
        unfilledframe.dstrect.y=(getH()/2.0f)-30;
        unfilledyes.dstrect.x=unfilledplaybutton.dstrect.x+60;
        unfilledyes.dstrect.y=unfilledplaybutton.dstrect.y+70;
        unfilledno.dstrect.x=filledquitbutton.dstrect.x+60;
        unfilledno.dstrect.y=filledquitbutton.dstrect.y+70;
        
        filledyes.dstrect = unfilledyes.dstrect;
        filledno.dstrect = unfilledno.dstrect;
        filledframe.dstrect = unfilledframe.dstrect;

        api::text warning(f,"Are you sure");
        warning.i.dstrect.x=unfilledframe.dstrect.x+30;
        warning.i.dstrect.y=unfilledframe.dstrect.y-60;
        bool quit=false;

        api::soundeffect laser("assets/sounds/laser-trim.mp3");
        api::soundeffect hitse("assets/sounds/hit.mp3");
        api::soundeffect explosion("assets/sounds/explosion.mp3");
        api::music chill("assets/music/chill-bg.mp3");
        api::music tuff("assets/music/tuff-bg.mp3");
        
        if (mute){
            laser.vol=0;
            hitse.vol=0;
            explosion.vol=0;
            chill.vol=0;
            tuff.vol=0;
        }
        else {
            laser.vol=3;
            hitse.vol=20;
            explosion.vol=50;
            chill.vol=128;
            tuff.vol=80;
            chill.play();
        }

        SDL_ShowWindow(getWindow());
        while (running){
            start=SDL_GetTicks64();
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        escape=true;
                        quit=true;
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
                escape=true;
                if (!play){
                    quit=true;
                }
            }
            mouse.x=mouselistener::getX();
            mouse.y=mouselistener::getY();
            if (play && !escape){
                if (keycontrols){
                    if (keylistener::isDown(SDLK_RIGHT) || keylistener::isDown(SDLK_d)){
                        b2Body_ApplyForceToCenter(body.get(), {40,0}, false);
                    }
                    if (keylistener::isDown(SDLK_LEFT) || keylistener::isDown(SDLK_a)){
                        b2Body_ApplyForceToCenter(body.get(), {-40,0}, false);
                    }
                    if (keylistener::isDown(SDLK_UP) || keylistener::isDown(SDLK_w)){
                        b2Body_ApplyForceToCenter(body.get(), {0,-60}, false);
                    }
                    if (keylistener::isDown(SDLK_DOWN) || keylistener::isDown(SDLK_s)){
                        b2Body_ApplyForceToCenter(body.get(), {-0,20}, false);
                    }
                }
                if (mousecontrols){
                    if (mouselistener::isDown(SDL_BUTTON_RIGHT)){
                        b2Body_ApplyForceToCenter(body.get(), {40,0}, false);
                    }
                    if (mouselistener::isDown(SDL_BUTTON_LEFT)){
                        b2Body_ApplyForceToCenter(body.get(), {-40,0}, false);
                    }
                    if (mouselistener::getScrly()<0){
                        b2Body_ApplyForceToCenter(body.get(), {0,-40}, false);
                    }
                    if (mouselistener::getScrly()>0){
                        b2Body_ApplyForceToCenter(body.get(), {-0,5}, false);
                    }
                }
                
                ast.speed=astspeed;
                if (score >= maxforupgrade){
                    bulletintervelmax-=1;
                    bulletspeed+=3;
                    maxforupgrade*=4;
                    if (!cheat){
                        astspeed++;
                        ast.size+=0.75;
                    }
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
                    if (cheat){
                        bullets.push_back({bullet.dstrect.x-((ship.dstrect.w/2.0f)-5), bullet.dstrect.y});
                        bullets.push_back({bullet.dstrect.x+(ship.dstrect.w/2.0f)-10, bullet.dstrect.y});
                    }
                    else {
                        if (score>=10){
                            bullets.push_back({bullet.dstrect.x-((ship.dstrect.w/2.0f)-5), bullet.dstrect.y});
                        }
                        if (score>=50){
                            bullets.push_back({bullet.dstrect.x+(ship.dstrect.w/2.0f)-10, bullet.dstrect.y});
                        }
                    }
                }

                if ((ship.dstrect.x+32)< 0 || (ship.dstrect.x+ship.dstrect.w-32)>getW()){
                    play=false;
                    b2Body_SetTransform(body.get(), {0,400/32.0f}, b2Body_GetRotation(body.get()));
                    ast.reset();
                    bullets.clear();
                    b2Body_SetLinearVelocity(body.get(), {0,0});
                    if (!mute){
                        chill.play();
                    }
                }
                else if ((ship.dstrect.y+32)< 0 || (ship.dstrect.y+ship.dstrect.h-32)>getH()){
                    play=false;
                    b2Body_SetTransform(body.get(), {0,400/32.0f}, b2Body_GetRotation(body.get()));
                    ast.reset();
                    bullets.clear();
                    b2Body_SetLinearVelocity(body.get(), {0,0});
                    if (!mute){
                        chill.play();
                    }
                }
                bool h=false;
                for (int i=0;i<bullets.size();i++){
                    SDL_Rect r={bullets[i].first, bullets[i].second, bullet.dstrect.w, bullet.dstrect.h};
                    if (ast.y > ast.im.dstrect.h){
                        if (SDL_HasIntersection(&r, &ast.im.dstrect)){
                            bullets[i].second-=(ast.im.dstrect.h+10);
                            hit=true;
                            i=bullets.size();
                            hitcount++;
                            h=true;
                        }
                    }
                }
                if (h){
                    if (!mute){
                        hitse.play();
                    }
                }
                if (!explode){
                    ast.update(distrib(gen)*100);
                }
                if ((hitcount-5)>=ast.size){
                    explode=true;
                    if (!mute){
                        explosion.play();
                    }
                }
                if (SDL_HasIntersection(&ast.im.dstrect, &ship.dstrect) && !explode && !cheat){
                    play=false;
                    b2Body_SetTransform(body.get(), {0,400/32.0f}, b2Body_GetRotation(body.get()));
                    ast.reset();
                    bullets.clear();
                    b2Body_SetLinearVelocity(body.get(), {0,0});
                    if (!mute){
                        chill.play();
                    }
                }
            }
            SDL_RenderClear(getRenderer());
            if (play && !escape){
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
                if (bullets.size()){
                    if (!mute){
                        laser.play();
                    }
                }
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
                if (escape){
                    warning.draw();
                    if (SDL_HasIntersection(&mouse, &unfilledframe.dstrect)){
                        filledframe.draw();
                    }
                    else {
                        unfilledframe.draw();
                    }
                    if (SDL_HasIntersection(&mouse, &unfilledyes.dstrect)){
                        filledyes.draw();
                        if (mouselistener::isPressed(SDL_BUTTON_LEFT)){
                            if (quit){
                                running=false;
                                quit=false;
                            }
                            else {
                                play=false;
                                b2Body_SetTransform(body.get(), {0,400/32.0f}, b2Body_GetRotation(body.get()));
                                ast.reset();
                                bullets.clear();
                                b2Body_SetLinearVelocity(body.get(), {0,0});
                                if (!mute){
                                    chill.play();
                                }
                            }
                            escape=false;
                        }
                    }
                    else {
                        unfilledyes.draw();
                    }
                    if (SDL_HasIntersection(&mouse, &unfilledno.dstrect)){
                        filledno.draw();
                        if (mouselistener::isPressed(SDL_BUTTON_LEFT)){
                            escape=false;
                            quit=false;
                        }
                    }
                    else {
                        unfilledno.draw();
                    }
                }
                else {
                    if (SDL_HasIntersection(&mouse, &unfilledplaybutton.dstrect)){
                        filledplaybutton.draw();
                        if (mouselistener::isPressed(SDL_BUTTON_LEFT)){
                            play=true;
                            score=0;
                            if (!mute){
                                chill.halt();
                                tuff.play();
                            }
                        }
                    }
                    else {
                        unfilledplaybutton.draw();
                    }
                    if (SDL_HasIntersection(&mouse, &unfilledquitbutton.dstrect)){
                        filledquitbutton.draw();
                        if (mouselistener::isPressed(SDL_BUTTON_LEFT)){
                            escape=true;
                            quit=true;
                        }
                    }
                    else {
                        unfilledquitbutton.draw();
                    }
                }
            }
            SDL_RenderPresent(getRenderer());
            if (play && !escape){
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
        if (file){
            int pos=0;
            std::ifstream filein("scores.txt");
            if (filein.is_open()){
                std::string line;
                int skipcount=0;
                while (filein >> line){
                    if (skipcount){
                        skipcount--;
                        continue;   
                    }
                    try {
                        auto i = std::stoi(line.substr(0,1));
                        if (i > pos){
                            pos=i;
                        }
                    }
                    catch(std::exception& e){}
                    skipcount=7;
                }
                filein.close();
            }
            pos++;
            auto endtime = std::chrono::steady_clock::now();
            auto a = std::chrono::duration_cast<std::chrono::seconds>(starttime.time_since_epoch());
            auto b = std::chrono::duration_cast<std::chrono::seconds>(endtime.time_since_epoch());
            std::ofstream file("scores.txt", std::ios::app);
            if (file.is_open()){
                file << pos << ":- Played for " << b.count()-a.count() << " seconds, with Score:- " << score << "\n";
                file.close();
            }
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
