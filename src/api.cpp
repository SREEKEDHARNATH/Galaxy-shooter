#include "api.hpp"
#include "SDL_error.h"
#include "SDL_mixer.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "SDL_ttf.h"
#include "core.hpp"
#include "SDL_image.h"
#include <iostream>
#include <memory>
#include <string>

namespace core::api {
    image::image(const char* name, bool load, bool autoset){
        set(name, load, autoset);
    }
    void image::set(const char* name, bool load, bool autoset){
        if (load && !std::string(name).empty()){
            auto tex = IMG_LoadTexture(getRenderer(), name);
            if (!tex){
                std::cerr << IMG_GetError() << "\n";
                return;
            }
            ptr.reset(tex, SDL_DestroyTexture);
            if (autoset){
                recallibrate();
            }
        }
    }
    void image::set(SDL_Texture *tex){
        if (tex){
            ptr.reset(tex, SDL_DestroyTexture);
            recallibrate();
        }
    }
    void image::set(Uint8 r, Uint8 g, Uint8 b, int w, int h, Uint8 depth){
        if (w<=0 || h<=0){
            std::cerr << "Width or height cant be 0 or negative\n";
        }
        else {
            auto surf = SDL_CreateRGBSurface(0, w, h, depth, 0,0,0,0);
            if (!surf){
                std::cerr << SDL_GetError() << "\n";
                return;
            }
            SDL_SetSurfaceBlendMode(surf, SDL_BLENDMODE_BLEND);
            auto colour=SDL_MapRGB(surf->format, r, g, b);
            SDL_FillRect(surf, NULL, colour);
            auto tex=SDL_CreateTextureFromSurface(getRenderer(), surf);
            if (!tex){
                std::cerr << SDL_GetError() << "\n";
                return;
            }
            ptr.reset(tex, SDL_DestroyTexture);
            recallibrate();
        }
    }
    void image::draw(){
        if (get()){
            if (dstrect.w<=0 || dstrect.h<=0){
                SDL_RenderCopy(getRenderer(), get().get(), NULL, NULL);
            }
            else {
                SDL_RenderCopy(getRenderer(), get().get(), NULL, &dstrect);
            }
        }
    }
    void image::recallibrate(){
        if (get().get()){
            SDL_QueryTexture(get().get(), NULL, NULL, &dstrect.w, &dstrect.h);
        }
    }
    std::shared_ptr<SDL_Texture> image::get(){
        return ptr;
    }

    font::font(const char *name, int ptsize, bool load){
        if (load){
            auto pt= TTF_OpenFont(name, ptsize);
            if (!pt){
                std::cerr << TTF_GetError() << "\n";
                return;
            }
            ptr.reset(pt, TTF_CloseFont);
        }
    }
    void font::set(const char *name, int ptsize){
        auto pt=TTF_OpenFont(name, ptsize);
        if (!pt){
            std::cerr << TTF_GetError() << "\n";
            return;
        }
        ptr.reset(pt, TTF_CloseFont);
    }
    void font::set(TTF_Font *fnt){
        if (fnt){
            ptr.reset(fnt, TTF_CloseFont);
        }
    }
    std::shared_ptr<TTF_Font> font::get(){
        return ptr;
    }

    text::text(font& fnt, const char* txt,SDL_Color fg, bool load) : i("", false){
        f.reset();
        setText(fnt,txt,fg);
    }
    void text::setText(font& f, const char *txt,SDL_Color fg){
        if (f.get()){
            st=txt;
            if (st.empty()){
                std::cerr << "Text cant be empty\n";
                return;
            }
            auto srf=TTF_RenderUTF8_Blended(f.get().get(), get(), fg);
            if (!srf){
                std::cerr << TTF_GetError() << "\n";
                return;
            }
            auto t=SDL_CreateTextureFromSurface(getRenderer(), srf);
            if (!t){
                std::cerr << SDL_GetError() << "\n";
                return;
            }
            i.set(t);
        }
    }
    void text::draw(){
        i.draw();
    }
    const char* text::get(){
        return st.c_str();
    }

    soundeffect::soundeffect(const char* name){
        auto pt = Mix_LoadWAV(name);
        if (!pt){
            std::cerr << Mix_GetError() << "\n";
            return;
        }
        ptr.reset(pt, Mix_FreeChunk);
    }
    void soundeffect::play(){
        if (ptr.get()){
            if (vol > 128){
                vol = 128;
            }
            else if (vol < 0){
                vol = 0;
            }
            if (vol != ptr.get()->volume){
                Mix_VolumeChunk(ptr.get(), vol);
            }
            bool play=true;
            if (channel != -1){
                play=Mix_Playing(channel);
            }
            if (Mix_Playing(-1) < 4){
                if (play){
                    channel = Mix_PlayChannel(-1, ptr.get(), false);
                    if (channel==-1){
                        std::cerr << Mix_GetError() << "\n";
                    }
                }
            }
        }
    }
    std::shared_ptr<Mix_Chunk> soundeffect::get(){
        return ptr;
    }

    music::music(const char* name){
        auto pt = Mix_LoadMUS(name);
        if (!pt){
            std::cerr << Mix_GetError() << "\n";
            return;
        }
        ptr.reset(pt, Mix_FreeMusic);
    }
    void music::play(bool loops){
        if (ptr.get()){
            if (vol > 100){
                vol = 100;
            }
            if (vol < 0){
                vol = 0;
            }
            if (vol != Mix_GetMusicVolume(ptr.get())){
                Mix_VolumeMusic(vol);
            }
            auto rescode = Mix_PlayMusic(ptr.get(), loops);
            if (rescode==-1){
                std::cerr << Mix_GetError() << "\n";
            }
        }
    }
    void music::pause(){
        if (ptr.get()){
            Mix_PauseMusic();
        }
    }
    void music::resume(){
        if (ptr.get()){
            Mix_ResumeMusic();
        }
    }
    void music::halt(){
        if (ptr.get()){
            Mix_HaltMusic();
        }
    }
    std::shared_ptr<Mix_Music> music::get(){
        return ptr;
    }
}