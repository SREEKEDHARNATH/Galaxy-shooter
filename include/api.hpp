#pragma once

#include "SDL_render.h"
#include "SDL_stdinc.h"
#include <memory>
namespace core::api {
    class image{
        std::shared_ptr<SDL_Texture> ptr;
        public:
            SDL_Rect dstrect={0,0,0,0};
            image(const char* name, bool load=true, bool autoset=true);
            void set(const char* name, bool load=true, bool autoset=true);
            void set(SDL_Texture* tex);
            void set(Uint8 r, Uint8 g, Uint8 b, int w, int h, Uint8 depth=32);
            void draw();
            void recallibrate();
    };
}