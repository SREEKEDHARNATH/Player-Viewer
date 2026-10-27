#include "api.hpp"
#include "Log.hpp"
#include "window.hpp"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>
namespace core::api {
    image::image(const char* name, bool autoset,bool load){
        if (load){
            SDL_Texture* tex= IMG_LoadTexture(getRenderer(), name);
            if (!tex){
                log(IMG_GetError(), LOG::WARN);
                return;
            }
            ptr.reset(tex, SDL_DestroyTexture);
            if (autoset){
                SDL_QueryTexture(tex, NULL, NULL, &dstrect.w, &dstrect.h);
            }
        }
    }

    void image::draw(){
        if (get().get()){
            if (dstrect.w == 0 || dstrect.h == 0){
                SDL_RenderCopy(getRenderer(), get().get(), NULL, NULL);
            }
            else{
                SDL_RenderCopy(getRenderer(), get().get(), NULL, &dstrect);
            }
        }
    }

    void image::setTex(SDL_Texture* tex, bool autoset){
        if (tex){
            ptr.reset(tex, SDL_DestroyTexture);
            if (autoset){
                SDL_QueryTexture(tex, NULL, NULL, &dstrect.w, &dstrect.h);
            }
        }
        else {
            log("Invalid Texture in settex image class", LOG::INFO);
        }
    }

    std::shared_ptr<SDL_Texture> image::get(){
        return ptr;
    }

    font::font(const char* name, int ptsize){
        ptr.reset(TTF_OpenFont(name, ptsize), TTF_CloseFont);
        if (!ptr.get()){
            log(TTF_GetError(), LOG::WARN);
            return;
        }
    }

    std::weak_ptr<TTF_Font> font::get(){
        return std::weak_ptr<TTF_Font>(ptr);
    }

    void text::init(const char* txt){
        std::string ta=txt;

        if (!ta.empty()){
            if (std::shared_ptr<TTF_Font> p = fo.lock()){
                if (p.get()){

                    SDL_Surface* surf=TTF_RenderUTF8_Blended(p.get(), txt, {255,255,255,255});
                    if (!surf){
                        log(TTF_GetError(), LOG::WARN);
                        return;
                    }
                    SDL_Texture* tex=SDL_CreateTextureFromSurface(getRenderer(), surf);
                    SDL_FreeSurface(surf);

                    if (!tex){
                        log(IMG_GetError(), LOG::WARN);
                        return;
                    }
                    img.setTex(tex);

                    st=txt;
                    empty=false;
                }
            }
            else {
                log("Invalid font", LOG::WARN);
            }
        }
        else {
           empty=true; 
        }
    }

    text::text(const char* txt, std::weak_ptr<TTF_Font> f) : img("",false,false){
        fo=f;
        init(txt);
    }

    void text::draw(){
        if (img.get()){
            if (!empty){
                img.draw();
            }
        }
    }

    void text::setText(const char* txt){
        init(txt);
    }

    std::string text::getText(){
        return st;
    }

    sound::sound(const char* name){
        Mix_Chunk* p=Mix_LoadWAV(name);
        if (!p){
            log(Mix_GetError(), LOG::WARN);            
            return;
        }
        ptr.reset(p, Mix_FreeChunk);
    }

    void sound::play(){
        if (ptr.get()){
            Mix_PlayChannel(-1, ptr.get(), false);
        }
    }

    void sound::setVol(int vol){
        if (ptr.get()){
            if (vol > 0){
                vol = std::min(vol, 100);
                Mix_VolumeChunk(ptr.get(), vol);
            }
        }
    }

    std::shared_ptr<Mix_Chunk> sound::get(){
        return ptr;
    }

    music::music(const char* name, bool load){
        if (load){
            Mix_Music* p = Mix_LoadMUS(name);
            if (!p){
                log(Mix_GetError(), LOG::WARN);
                return;
            }
            ptr.reset(p, Mix_FreeMusic);
        }
    }

    void music::setMusic(const char* name){
        if (ptr.get()){
            halt();
        }
        Mix_Music* p = Mix_LoadMUS(name);
        if (!p){
            log(Mix_GetError(), LOG::WARN);
            return;
        }
        ptr.reset(p, Mix_FreeMusic);
    }

    void music::play(bool loops){
        if (ptr.get()){
            Mix_PlayMusic(ptr.get(), loops);
        }
    }

    void music::pause(){
        if (ptr.get()){
            if (!Mix_PausedMusic()){
                Mix_PauseMusic();
            }
        }
    }

    void music::resume(){
        if (ptr.get()){
            if (Mix_PausedMusic()){
                Mix_ResumeMusic();
            }
        }
    }

    void music::halt(){
        if (ptr.get()){
            Mix_HaltMusic();
        }
    }

    void music::skip(double time){
        if (ptr.get()){
            double length = getTotalDuration();
            double start = getCurrentDuration();

            if (time > 0){
                if ((length-start) >= time){
                    Mix_SetMusicPosition(start+time);
                }
            }
            else if (time < 0){
                if ((start+time) >= 0){
                    Mix_SetMusicPosition(start+time);
                }
            }
        }
    }

    void music::setVol(int vol){
        if (ptr.get()){
            if (vol > 0){
                vol = std::min(vol, 100);
                Mix_VolumeMusic(vol);
            }
        }
    }

    void music::destroy(){
        ptr.reset();
    }

    double music::getTotalDuration(){
        if (ptr.get()){
            return Mix_MusicDuration(ptr.get());
        }
        return -1;
    }

    double music::getCurrentDuration(){
        if (ptr.get()){
            return Mix_GetMusicPosition(ptr.get());
        }
        return -1;
    }
    
    std::shared_ptr<Mix_Music> music::get(){
        return ptr;
    }
}
