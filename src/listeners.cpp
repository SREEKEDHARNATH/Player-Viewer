#include "listeners.hpp"
#include "Log.hpp"
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_scancode.h>
namespace core::keylistener {
    bool keys[SDL_NUM_SCANCODES]={false};
    bool prvkeys[SDL_NUM_SCANCODES]={false};
    
    void reg(SDL_KeyboardEvent key){
        auto sc=key.keysym.scancode;
        if (sc>0 && sc<SDL_NUM_SCANCODES){
            keys[sc]=(key.state==SDL_PRESSED);
        }
    }

    bool isPressed(SDL_KeyCode k){
        return isPressed(SDL_GetScancodeFromKey(k));
    }
    bool isPressed(SDL_Scancode sc){
        if (sc>0 && sc<SDL_NUM_SCANCODES){
            return keys[sc] && !prvkeys[sc];
        }
        log("Invalid key", LOG::WARN);
        return false;
    }
    
    bool isDown(SDL_KeyCode k){
        return isDown(SDL_GetScancodeFromKey(k));
    }
    bool isDown(SDL_Scancode sc){
        if (sc>0 && sc<SDL_NUM_SCANCODES){
            return keys[sc];
        }
        log("Invalid key", LOG::WARN);
        return false;
    }
    void update(){
        for (int i=0;i<SDL_NUM_SCANCODES;i++){
            prvkeys[i]=keys[i];
        }
    }
}

namespace core::mouselistener {
    int x=0,y=0;
    bool buttons[3]={false};
    bool prvbuttons[3]={false};
    float scrlx=0,scrly;

    void reg(SDL_MouseMotionEvent motion){
        x=motion.x;
        y=motion.y;
    }
    void reg(SDL_MouseButtonEvent button){
        auto b=button.button;
        if (b<4){
            buttons[b-1]=(button.state==SDL_PRESSED);
        }
    }
    void reg(SDL_MouseWheelEvent wheel){
        auto upx=wheel.x;
        auto upy=wheel.y;

        if (wheel.direction==SDL_MOUSEWHEEL_FLIPPED){
            upx*=-1;
            upy*=-1;
        }

        scrlx+=upx;
        scrly+=upy;
    }
    
    bool isPressed(int b){
        if (b>0 && b<4){
            return buttons[b-1] && !prvbuttons[b-1];
        }
        log("Invalid Button", LOG::WARN);
        return false;
    }
    bool isDown(int b){
        if (b>0 && b<4){
            return buttons[b-1];
        }
        log("Invalid Button", LOG::WARN);
        return false;
    }
    
    int getX(){
        return x;
    }
    int getY(){
        return y;
    }
    
    float getScrlX(){
        return scrlx;
    }
    float getScrlY(){
        return scrly;
    }

    void update(){
        for (int i=0;i<3;i++){
            prvbuttons[i]=buttons[i];
        }
    }
}