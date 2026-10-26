#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_video.h>
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

        bool running=true;
        SDL_Event e;
        api::font f("assets/fonts/Montserrat-BoldItalic.ttf", 32);
        api::text t("Hi", f.get());
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
            running=!keylistener::isPressed(SDLK_ESCAPE);

            SDL_RenderClear(getRenderer());
            t.draw();
            SDL_RenderPresent(getRenderer());
            
            keylistener::update();
            mouselistener::update();
            SDL_Delay(16);
        }
    }

    std::pair<bool, const char*> init(){
        int img_flags=IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP;
        int sound_flags=MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_OPUS;
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