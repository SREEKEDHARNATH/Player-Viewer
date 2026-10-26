#pragma once

#include <SDL.h>
#include <string>

namespace core {
    void run();
    void quit();

    int& getW();
    int& getH();
    
    std::string& getTitle();

    SDL_Window*& getWindow();
    SDL_Renderer*& getRenderer();
    
}