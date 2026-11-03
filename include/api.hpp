#pragma once

#include "SDL_mixer.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include <memory>
#include <string>
#include "SDL_ttf.h"

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
            std::shared_ptr<SDL_Texture> get();
    };
    class font{
        std::shared_ptr<TTF_Font> ptr;
        public:
            font(const char* name, int ptsize=32, bool load=true);
            void set(const char* name, int ptsize=32);
            void set(TTF_Font* fnt);
            std::shared_ptr<TTF_Font> get();
    };
    class text{
        std::shared_ptr<TTF_Font> f;
        std::string st;
        public:
            image i;
            text(font& fnt, const char* txt,SDL_Color fg={255,255,255,255}, bool load=true);
            void setText(font& f,const char* txt,SDL_Color fg={255,255,255,255});
            void draw();
            const char* get();
    };
    class soundeffect{
        std::shared_ptr<Mix_Chunk> ptr;
        public:
            int vol=80;
            soundeffect(const char* name);
            void play();
            std::shared_ptr<Mix_Chunk> get();
    };
    class music{
        std::shared_ptr<Mix_Music> ptr;
        public:
            int vol=50;
            music(const char* name);
            void play(bool loops=true);
            void pause();
            void resume();
            void halt();
            std::shared_ptr<Mix_Music> get();
    };
    
}