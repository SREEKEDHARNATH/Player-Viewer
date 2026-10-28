#include "api.hpp"
#include "Log.hpp"
#include "window.hpp"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <algorithm>
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

    music::music(const char* name, bool load){
        if (load){
            ptr=std::make_shared<SoLoud::WavStream>();
    
            int error_code=ptr->load(name);
            if (error_code){
                if (error_code==1){
                    log("Invalid Parameter", LOG::WARN);
                }
                else if(error_code==2){
                    log("File Not Found", LOG::WARN);
                }
                else if(error_code==3){
                    log("File load failed", LOG::WARN);
                }
                else {
                    log("Unknown Error", LOG::WARN);
                }
            }
        }
    }
    void music::destroy(){
        ptr.get()->stop();
        handle=0;
    }
    void music::set(const char* name){
        if (ptr.get()){
            destroy();
        }
        ptr=std::make_shared<SoLoud::WavStream>();

        int error_code=ptr->load(name);
        if (error_code){
            if (error_code==1){
                log("Invalid Parameter", LOG::WARN);
            }
            else if(error_code==2){
                log("File Not Found", LOG::WARN);
            }
            else if(error_code==3){
                log("File load failed", LOG::WARN);
            }
            else {
                log("Unknown Error", LOG::WARN);
            }
            return;
        }
    }
    void music::play(SoLoud::Soloud* soloud){
        if (ptr.get()){
            auto* a=ptr.get();
            a->setLooping(true);
            handle = soloud->play(*a);
        }
    }
    void music::pause(SoLoud::Soloud* soloud){
        if (ptr.get()){
            if (handle){
                soloud->setPause(handle, true);
            }
        }
    }
    void music::resume(SoLoud::Soloud* soloud){
        if (ptr.get()){
            auto* a=ptr.get();
            a->setLooping(true);
            if (handle){
                soloud->setPause(handle, false);
            }
        }
    }
    void music::halt(SoLoud::Soloud* soloud){
        if (ptr.get()){
            auto* a=ptr.get();
            a->setLooping(true);
            soloud->stop(handle);
            handle=0;
        }
    }
    void music::setVol(SoLoud::Soloud* soloud, int vol){
        if (ptr.get()){
            if (vol >= 0){
                if (handle){
                    soloud->setVolume(handle, std::min((vol+15)/20, 50));
                }
            }
        }
    }
    void music::setSpeed(SoLoud::Soloud* soloud, double speed){
        if (ptr.get()){
            if (handle){
                soloud->setRelativePlaySpeed(handle, speed);
            }
        }
    }
    void music::skip(SoLoud::Soloud* soloud, double sk){
        if (ptr.get()){
            if (handle){
                auto a=getTotalDuration();
                auto b=getCurrentDuration(soloud);
                if (sk > 0){
                    if ((a-b) >= sk){
                        soloud->seek(handle, b+sk);
                    }
                }
                if (sk < 0){
                    if (b+sk >= 0){
                        soloud->seek(handle, b+sk);
                    }
                }
                
            }
        }
    }
    double music::getCurrentDuration(SoLoud::Soloud* soloud){
        if (ptr.get()){
            if (handle){
                return soloud->getStreamPosition(handle);
            }
        }
        return -1;
    }
    double music::getTotalDuration(){
        return ptr.get()->getLength();
    }
    void music::setDuration(SoLoud::Soloud* soloud, float time){
        if (ptr.get()){
            if (handle){
                soloud->seek(handle, time);
            }
        }
    }
    int music::getVol(){
        return ptr.get()->mVolume;
    }
    std::shared_ptr<SoLoud::WavStream> music::get(){
        return ptr;
    }
}
