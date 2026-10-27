#include <SDL.h>
#include <SDL_clipboard.h>
#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_video.h>
#include <string>
#include <utility>
#include "window.hpp"
#include "Log.hpp"
#include "api.hpp"
#include "listeners.hpp"

namespace core {
    std::pair<bool, const char*> init();
    void run(){
        auto i=init();
        if (!i.first)    {
            log(i.second, LOG::ERROR);
        }

        SDL_Surface* surf=IMG_Load("assets/cursors/Cursor.png");
        SDL_Cursor* cursor=nullptr;
        if (surf){
            cursor = SDL_CreateColorCursor(surf, 0, 0);
            if (cursor){
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetCursor(cursor);
                SDL_ShowCursor(SDL_ENABLE);
            }
            else {
                log(SDL_GetError(), LOG::WARN);    
            }
            SDL_FreeSurface(surf);
        }
        else {
            log(SDL_GetError(), LOG::WARN);
        }
        bool running=true;
        SDL_Event e;
        api::font f("assets/fonts/Montserrat-BoldItalic.ttf", 32);
        api::text t("", f.get());
        t.img.dstrect.y=300;
        api::music m("", false);
        std::string txt="";
        bool caps=false,shift=false,ctrl=false;
        bool paused=false;
        Uint8 vol=100;
        bool muted=false;
        bool txtinput=true;

        api::image back("assets/images/back-button.jpg");
        SDL_Rect mouse={0,0,1,1};

        api::image up("assets/images/volume-up.jpg");
        api::image down("assets/images/volume-down.jpg");
        
        api::image play("assets/images/play.jpg");
        api::image pause("assets/images/pause.jpg");
        play.dstrect.x=(getW()/2)-32;
        play.dstrect.y=(getH()/2)-32;
        play.dstrect.w*=2;
        play.dstrect.h*=2;
        pause.dstrect.x=(getW()/2)-32;
        pause.dstrect.y=(getH()/2)-32;
        pause.dstrect.w*=2;
        pause.dstrect.h*=2;

        api::text volume(std::string(std::to_string(vol)+"%").c_str(),f.get());
        volume.img.dstrect.x=getW()-200;
        volume.img.dstrect.y=getH()-200;
        
        up.dstrect.x=getW()-210;
        up.dstrect.y=getH()-250;
        up.dstrect.w*=2;
        up.dstrect.h*=2;
        
        down.dstrect.x=getW()-210;
        down.dstrect.y=getH()-150;
        down.dstrect.w*=2;
        down.dstrect.h*=2;

        api::image skipositive("assets/images/skip10s.jpg");
        skipositive.dstrect.x=(getW()/2)+224;
        skipositive.dstrect.y=(getH()/2)-32;
        skipositive.dstrect.w*=2;
        skipositive.dstrect.h*=2;

        api::image skinegative("assets/images/skip-10s.jpg");
        skinegative.dstrect.x=(getW()/2)-256;
        skinegative.dstrect.y=(getH()/2)-32;
        skinegative.dstrect.w*=2;
        skinegative.dstrect.h*=2;

        api::image sliderbutton("assets/images/slider-button.png");
        sliderbutton.dstrect.y=(getH()/2)+300;

        api::image sliderleft("assets/images/left-end-slider.jpg");
        sliderleft.dstrect.y=(getH()/2)+307;
        sliderleft.dstrect.h*=4;

        api::image slideright("assets/images/right-end-slider.jpg");
        slideright.dstrect.y=(getH()/2)+307;
        slideright.dstrect.h*=4;
        slideright.dstrect.x=getW()-slideright.dstrect.w;

        api::image slidermiddle("assets/images/middle-slider.jpg");
        slidermiddle.dstrect.y=(getH()/2)+307;
        slidermiddle.dstrect.h*=4;
        slidermiddle.dstrect.x=sliderleft.dstrect.w;
        slidermiddle.dstrect.w=getW()-(sliderleft.dstrect.w+slideright.dstrect.w);

        while (running) {
            while (SDL_PollEvent(&e)){
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
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                        mouselistener::reg(e.button);
                        break;
                    case SDL_MOUSEWHEEL:
                        mouselistener::reg(e.wheel);
                        break;
                    
                }
            }
            mouse.x=mouselistener::getX();
            mouse.y=mouselistener::getY();
            if (keylistener::isPressed(SDLK_ESCAPE)){
                running=false;
            }
            if (keylistener::isPressed(SDLK_CAPSLOCK)){
                caps=!caps;
            }
            if (keylistener::isDown(SDLK_LSHIFT)){
                shift=true;
            }
            else if (keylistener::isDown(SDLK_RSHIFT)){
                shift=true;
            }
            else {
                shift=false;
            }
            if (keylistener::isDown(SDLK_LCTRL)){
                ctrl=true;
            }
            else if (keylistener::isDown(SDLK_RCTRL)){
                ctrl=true;
            }
            else {
                ctrl=false;
            }
            
            if (txtinput){
                if (!ctrl){
                    for (int i=32;i<=122;i++){
                        if (i==33){
                            i=44;
                        }
                        if (i == 58 || i == 60 || i == 62 || i == 63 || i == 64 || i == 94 || i == 95){
                            continue;
                        }
                        if (i>64 && i<91){
                            if (keylistener::isPressed((SDL_KeyCode)(i+32))){
                                if (caps || shift){
                                    SDL_KeyboardEvent key;
                                    key.state=SDL_RELEASED;
                                    key.keysym.scancode=SDL_GetScancodeFromKey((SDL_KeyCode)(i+32));
                                    keylistener::reg(key);
                                    txt+=(char)(i);
                                }
                            }
                        }
                        else {
                            if (keylistener::isPressed((SDL_KeyCode)i)){
                                txt+=(char)i;
                            }
                        }
                    }
                }
                if (ctrl && keylistener::isPressed(SDLK_c)){
                    SDL_SetClipboardText(txt.c_str());
                }
                if (ctrl && keylistener::isPressed(SDLK_v)){
                    auto c=SDL_GetClipboardText();
                    txt+=c;
                    SDL_free(c);
                }
                if (keylistener::isPressed(SDLK_BACKSPACE)){
                    if (txt=="Invalid!"){
                        txt="";
                    }
                    else if(shift || ctrl){
                        txt="";
                    }
                    else if (txt.size()>1){
                        txt=txt.substr(0,txt.size()-1);
                    }
                    else if(txt.size()==1){
                        txt="";
                    }
                }
                if (keylistener::isPressed(SDL_SCANCODE_RETURN)){
                    m.setMusic(txt.c_str());
                    if (!m.get()){
                        txt="Invalid!";
                    }
                    else {
                        m.play();
                        txtinput=false;
                    }
                }
                if (txt!=t.getText()){
                    t.setText(txt.c_str());
                }
            }
            else {
                if (mouselistener::isPressed(SDL_BUTTON_LEFT)){
                    if (SDL_HasIntersection(&play.dstrect, &mouse)){
                        paused=!paused;
                        if (paused){
                            m.pause();
                        }
                        else {
                            m.resume();
                        }
                    }
                    if (SDL_HasIntersection(&back.dstrect, &mouse)){
                        txtinput=true;
                        m.destroy();
                    }
                    if (SDL_HasIntersection(&up.dstrect, &mouse)){
                        if (vol < 96){
                            vol+=5;
                            m.setVol(vol);
                        }
                    }
                    if (SDL_HasIntersection(&down.dstrect, &mouse)){
                        if (vol > 4){
                            vol-=5;
                            m.setVol(vol);
                        }
                    }
                    if (SDL_HasIntersection(&skipositive.dstrect, &mouse)){
                        m.skip(10);
                    }
                    if (SDL_HasIntersection(&skinegative.dstrect, &mouse)){
                        m.skip(-10);
                    }
                }
                if ((SDL_HasIntersection(&mouse, &slidermiddle.dstrect) || SDL_HasIntersection(&mouse, &slideright.dstrect) || SDL_HasIntersection(&mouse, &sliderleft.dstrect)) && mouselistener::isDown(SDL_BUTTON_LEFT)){
                    m.pause();
                    paused=true;
                    sliderbutton.dstrect.x=mouse.x;
                    Mix_SetMusicPosition((mouse.x/(double)getW())*m.getTotalDuration());
                }
                if (keylistener::isPressed(SDLK_p) || keylistener::isPressed(SDLK_SPACE)){
                    paused=!paused;
                    if (paused){
                        m.pause();
                    }
                    else {
                        m.resume();
                    }
                }
                if (keylistener::isPressed(SDLK_m)){
                    muted=!muted;
                    if (muted){
                        m.setVol(0);
                    }
                    else {
                        m.setVol(vol);
                    }
                }
                if (keylistener::isPressed(SDLK_RIGHT)){
                    m.skip(10);
                }   
                if (keylistener::isPressed(SDLK_LEFT)){
                    m.skip(-10);
                }
                if (keylistener::isPressed(SDLK_UP)){
                    if (vol < 96){
                        vol+=5;
                        m.setVol(vol);
                    }
                }
                if (keylistener::isPressed(SDLK_DOWN)){
                    if (vol > 4){
                        vol-=5;
                        m.setVol(vol);
                    }
                }
                if (keylistener::isPressed(SDLK_b)){
                    txtinput=true;
                    m.destroy();
                }
                if (muted){
                    volume.setText("0%");
                }
                else if (std::to_string(vol) != volume.getText().substr(0,volume.getText().size()-1)){
                    volume.setText(std::string(std::to_string(vol)+"%").c_str());
                }
            }
            
            SDL_RenderClear(getRenderer());
            if (txtinput){
                t.draw();
            }
            else {
                if (m.get()){
                   sliderbutton.dstrect.x=getW()*(m.getCurrentDuration()/m.getTotalDuration());
                   if (m.getCurrentDuration()>=m.getTotalDuration()){
                        Mix_SetMusicPosition(0);
                   }
                }
                skipositive.draw();
                skinegative.draw();
                back.draw();
                up.draw();
                down.draw();
                volume.draw();
                sliderleft.draw();
                slideright.draw();
                slidermiddle.draw();
                sliderbutton.draw();
                if (paused){
                    pause.draw();
                }
                else {
                    play.draw();
                }
            }
            SDL_RenderPresent(getRenderer());
            
            keylistener::update();
            mouselistener::update();
            SDL_Delay(16);
        }
        //auto curs=SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        SDL_SetCursor(NULL);
        if (cursor){
            SDL_FreeCursor(cursor);
        }
        
    }

    std::pair<bool, const char*> init(){
        int img_flags=IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP;
        int sound_flags=MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_OPUS | MIX_INIT_FLAC;
        if (SDL_Init(SDL_INIT_EVERYTHING)){
            return {false, SDL_GetError()};
        }
        if ((IMG_Init(img_flags) & img_flags) != img_flags){
            return {false, IMG_GetError()};
        }
        if (TTF_Init()){
            return {false, TTF_GetError()};
        }
        if ((Mix_Init(sound_flags) & sound_flags) != sound_flags){
            return {false, Mix_GetError()};
        }
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)){
            return {false, Mix_GetError()};
        }
        getWindow()=SDL_CreateWindow(getTitle().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, getW(), getH(), 0);
        if (!getWindow()){
            return {false, SDL_GetError()};
        }
        getRenderer()=SDL_CreateRenderer(getWindow(), -1, 0);
        if (!getRenderer()){
            return {false, SDL_GetError()};
        }
        SDL_Surface* logo = IMG_Load("assets/images/logo.jpg");
        if (!logo){
            return {false, SDL_GetError()};
        }
        SDL_SetWindowIcon(getWindow(), logo);
        SDL_FreeSurface(logo);
        return {true, "All great"};
    }
    void quit(){
        if (getWindow()){
            SDL_DestroyWindow(getWindow());
        }
        if (getRenderer()){
            SDL_DestroyRenderer(getRenderer());
        }
        Mix_CloseAudio();
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    int& getW(){
        static int w=1280;
        return w;
    }
    int& getH(){
        static int h=720;
        return h;
    }
    
    std::string& getTitle(){
        static std::string title="Hello";
        return title;
    }

    SDL_Window*& getWindow(){
        static SDL_Window* w=nullptr;
        return w;
    }
    SDL_Renderer*& getRenderer(){
        static SDL_Renderer* rend=nullptr;
        return rend;
    }
    
}