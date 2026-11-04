#include "listeners.hpp"

#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "SDL_scancode.h"
namespace core::keylistener {
    bool keys[SDL_NUM_SCANCODES]={false};
    bool prvkeys[SDL_NUM_SCANCODES]={false};
    
    void reg(SDL_KeyboardEvent key){
        auto sc = key.keysym.scancode;
        if (sc > 0 && sc < SDL_NUM_SCANCODES){
            keys[sc]=(key.state==SDL_PRESSED);
        }
    }

    bool isPressed(SDL_KeyCode k){
        return isPressed(SDL_GetScancodeFromKey(k));
    }
    bool isPressed(SDL_Scancode sc){
        if (sc > 0 && sc < SDL_NUM_SCANCODES){
            return keys[sc] && !prvkeys[sc];
        }
        return false;
    }
    
    bool isDown(SDL_KeyCode k){
        return isDown(SDL_GetScancodeFromKey(k));
    }
    bool isDown(SDL_Scancode sc){
        if (sc > 0 && sc < SDL_NUM_SCANCODES){
            return keys[sc];
        }
        return false;
    }

    void update(){
        //i starts at 1 because 0 in sdl scancode is unknown and i dont register that too
        for (int i=1;i<SDL_NUM_SCANCODES;i++){
            prvkeys[i]=keys[i];
        }
    }
}

namespace core::mouselistener {
    int x=0,y=0;
    float scrlx=0,scrly=0;
    bool buttons[3]={false,false,false};
    bool prvbuttons[3]={false,false,false};

    void reg(SDL_MouseMotionEvent m){
        x=m.x;
        y=m.y;
    }
    void reg(SDL_MouseButtonEvent b){
        if (b.button > 0 && b.button < 4){
            buttons[b.button-1]=(b.state==SDL_PRESSED);
        }
    }
    void reg(SDL_MouseWheelEvent w){
        scrlx=w.preciseX;
        scrly=w.preciseY;

        if (w.direction==SDL_MOUSEWHEEL_FLIPPED){
            scrlx*=-1;
            scrly*=-1;
        }
    }
    
    bool isPressed(int b){
        if (b > 0 && b < 4){
            return buttons[b-1] && !prvbuttons[b-1];
        }
        return false;
    }
    bool isDown(int b){
        if (b > 0 && b < 4){
            return buttons[b-1];
        }
        return false;
    }
    
    int getX(){
        return x;
    }
    int getY(){
        return y;
    }

    float getScrlx(){
        return scrlx;
    }
    float getScrly(){
        return scrly;
    }
    void update(){
        for (int i=0;i<3;i++){
            prvbuttons[i]=buttons[i];
        }
        if (mouselistener::isDown(SDL_BUTTON_MIDDLE)){
            scrlx=0;
            scrly=0;
        }
    }
}
