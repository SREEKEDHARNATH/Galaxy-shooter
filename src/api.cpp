#include "api.hpp"
#include "SDL_error.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "core.hpp"
#include "SDL_image.h"
#include <iostream>

namespace core::api {
    image::image(const char* name, bool load, bool autoset){
        set(name, load, autoset);
    }
    void image::set(const char* name, bool load, bool autoset){
        if (load && std::string(name).empty()){
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
        if (ptr.get()){
            if (dstrect.w<=0 || dstrect.h<=0){
                SDL_RenderCopy(getRenderer(), ptr.get(), NULL, NULL);
            }
            else {
                SDL_RenderCopy(getRenderer(), ptr.get(), NULL, &dstrect);
            }
        }
    }
    void image::recallibrate(){
        if (ptr.get()){
            SDL_QueryTexture(ptr.get(), NULL, NULL, &dstrect.w, &dstrect.h);
        }
    }
}