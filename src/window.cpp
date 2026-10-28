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
#include <SDL_video.h>
#include <string>
#include <utility>
#include "window.hpp"
#include "Log.hpp"
#include "api.hpp"
#include "listeners.hpp"
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

namespace core {
    std::pair<bool, const char*> init();
    void run(){
        auto i=init();
        if (!i.first)    {
            log(i.second, LOG::ERROR);
        }
        SoLoud::Soloud soloud;
        soloud.init();
        
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
        Uint8 vol=30;
        bool muted=false;
        bool txtinput=true;

        api::image close("assets/images/close.png");
        close.dstrect.x=getW()-close.dstrect.w;

        api::image noforward("assets/images/forward-skip.png");
        noforward.dstrect.y=getH()-200;
        noforward.dstrect.x=getW()-300;
        noforward.dstrect.w*=2;
        noforward.dstrect.h*=2;

        api::image forward25("assets/images/forward-skip25.png");
        forward25.dstrect.w*=2;
        forward25.dstrect.h*=2;
        forward25.dstrect.y=getH()-250;
        forward25.dstrect.x=getW()-300;

        api::image forward50("assets/images/forward-skip50.png");
        forward50.dstrect.w*=2;
        forward50.dstrect.h*=2;
        forward50.dstrect.y=getH()-300;
        forward50.dstrect.x=getW()-300;

        api::image forward75("assets/images/forward-skip75.png");
        forward75.dstrect.w*=2;
        forward75.dstrect.h*=2;
        forward75.dstrect.y=getH()-350;
        forward75.dstrect.x=getW()-300;

        api::image forward1("assets/images/forward-skip1.png");
        forward1.dstrect.w*=2;
        forward1.dstrect.h*=2;
        forward1.dstrect.y=getH()-400;
        forward1.dstrect.x=getW()-300;

        api::image forward125("assets/images/forward-skip125.png");
        forward125.dstrect.w*=2;
        forward125.dstrect.h*=2;
        forward125.dstrect.y=getH()-450;
        forward125.dstrect.x=getW()-300;

        api::image forward150("assets/images/forward-skip150.png");
        forward150.dstrect.w*=2;
        forward150.dstrect.h*=2;
        forward150.dstrect.y=getH()-500;
        forward150.dstrect.x=getW()-300;

        api::image forward175("assets/images/forward-skip175.png");
        forward175.dstrect.w*=2;
        forward175.dstrect.h*=2;
        forward175.dstrect.y=getH()-550;
        forward175.dstrect.x=getW()-300;

        api::image forward2("assets/images/forward-skip2.png");
        forward2.dstrect.w*=2;
        forward2.dstrect.h*=2;
        forward2.dstrect.y=getH()-600;
        forward2.dstrect.x=getW()-300;

        SDL_Rect combinedrect = forward2.dstrect;
        combinedrect.h*=8;
        bool showmenu=false;

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
                    m.set(txt.c_str());
                    if (!m.get()){
                        txt="Invalid!";
                    }
                    else {
                        m.play(&soloud);
                        txtinput=false;
                    }
                }
                if (txt!=t.getText()){
                    t.setText(txt.c_str());
                }
            }
            else {
                if (mouselistener::isPressed(SDL_BUTTON_LEFT)){
                    if (SDL_HasIntersection(&forward25.dstrect, &mouse)){
                        m.setSpeed(&soloud, 0.25);
                    }
                    if (SDL_HasIntersection(&forward50.dstrect, &mouse)){
                        m.setSpeed(&soloud, 0.50);
                    }
                    if (SDL_HasIntersection(&forward75.dstrect, &mouse)){
                        m.setSpeed(&soloud, 0.75);
                    }
                    if (SDL_HasIntersection(&forward1.dstrect, &mouse)){
                        m.setSpeed(&soloud, 1);
                    }
                    if (SDL_HasIntersection(&forward125.dstrect, &mouse)){
                        m.setSpeed(&soloud, 1.25);
                    }
                    if (SDL_HasIntersection(&forward150.dstrect, &mouse)){
                        m.setSpeed(&soloud, 1.50);
                    }
                    if (SDL_HasIntersection(&forward175.dstrect, &mouse)){
                        m.setSpeed(&soloud, 1.75);
                    }
                    if (SDL_HasIntersection(&forward2.dstrect, &mouse)){
                        m.setSpeed(&soloud, 2.0);
                    }
                    if (SDL_HasIntersection(&play.dstrect, &mouse)){
                        paused=!paused;
                        if (paused){
                            m.pause(&soloud);
                        }
                        else {
                            m.resume(&soloud);
                        }
                    }
                    if (SDL_HasIntersection(&back.dstrect, &mouse)){
                        txtinput=true;
                        m.destroy();
                    }
                    if (SDL_HasIntersection(&up.dstrect, &mouse)){
                        if (vol < 96){
                            vol+=5;
                        }
                        muted=false;
                    }
                    if (SDL_HasIntersection(&down.dstrect, &mouse)){
                        if (vol > 4){
                            vol-=5;
                            muted=false;
                        }
                        else {
                            muted=true;
                        }
                    }
                    if (SDL_HasIntersection(&skipositive.dstrect, &mouse)){
                        m.skip(&soloud,10);
                    }
                    if (SDL_HasIntersection(&skinegative.dstrect, &mouse)){
                        m.skip(&soloud,-10);
                    }
                }
                if (SDL_HasIntersection(&mouse, &noforward.dstrect)){
                    showmenu=true;
                }
                else if (SDL_HasIntersection(&mouse, &combinedrect));
                else {
                    showmenu=false;
                }
                if (SDL_HasIntersection(&mouse, &slidermiddle.dstrect) || SDL_HasIntersection(&mouse, &slideright.dstrect) || SDL_HasIntersection(&mouse, &sliderleft.dstrect)){
                    if (mouselistener::isPressed(SDL_BUTTON_LEFT)){
                        //m.pause(&soloud);
                        //paused=true;
                        sliderbutton.dstrect.x=mouse.x;
                        m.setDuration(&soloud, (mouse.x/(float)getW())*m.getTotalDuration());
                    }
                }
                if (keylistener::isPressed(SDLK_p) || keylistener::isPressed(SDLK_SPACE)){
                    paused=!paused;
                    if (paused){
                        m.pause(&soloud);
                    }
                    else {
                        m.resume(&soloud);
                    }
                }
                if (keylistener::isPressed(SDLK_m)){
                    muted=true;
                }
                if (keylistener::isPressed(SDLK_RIGHT)){
                    m.skip(&soloud,10);
                }   
                if (keylistener::isPressed(SDLK_LEFT)){
                    m.skip(&soloud,-10);
                }
                if (keylistener::isPressed(SDLK_UP)){
                    if (vol < 96){
                        vol+=5;
                    }
                    muted=false;
                }
                if (keylistener::isPressed(SDLK_DOWN)){
                    if (vol > 4){
                        vol-=5;
                        muted=false;
                    }
                    else {
                        muted=true;
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
            if (mouselistener::isPressed(SDL_BUTTON_LEFT) && SDL_HasIntersection(&mouse, &close.dstrect)){
                running=false;
            }
            close.draw();
            if (txtinput){
                t.draw();
            }
            else {
                if (muted){
                    if (m.getVol()){
                        m.setVol(&soloud, 0);
                    }
                }
                else {
                    if (m.getVol() != vol){
                        m.setVol(&soloud, vol);
                    }
                }
                noforward.draw();
                if (showmenu){
                    forward25.draw();
                    forward50.draw();
                    forward75.draw();
                    forward1.draw();
                    forward125.draw();
                    forward150.draw();
                    forward175.draw();
                    forward2.draw();
                }
                if (m.get()){
                    sliderbutton.dstrect.x=getW()*(m.getCurrentDuration(&soloud)/m.getTotalDuration());
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
        SDL_SetCursor(NULL);
        if (cursor){
            SDL_FreeCursor(cursor);
        }
        soloud.deinit();
    }

    std::pair<bool, const char*> init(){
        int img_flags=IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP;
        if (SDL_Init(SDL_INIT_EVERYTHING)){
            return {false, SDL_GetError()};
        }
        if ((IMG_Init(img_flags) & img_flags) != img_flags){
            return {false, IMG_GetError()};
        }
        if (TTF_Init()){
            return {false, TTF_GetError()};
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