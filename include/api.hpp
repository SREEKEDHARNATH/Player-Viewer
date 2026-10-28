#pragma once

#include "soloud/soloud.h"
#include "soloud/soloud_wavstream.h"
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>
namespace core::api {
    class image{
        std::shared_ptr<SDL_Texture> ptr;
        public:
            SDL_Rect dstrect={0};

            image(const char* name, bool autoset=true,bool load=true); 
            void draw();
            void setTex(SDL_Texture* tex, bool autoset=true);
            
            std::shared_ptr<SDL_Texture> get();
    };

    class font{
        std::shared_ptr<TTF_Font> ptr; 
        public:
            font(const char* name, int ptsize);
            std::weak_ptr<TTF_Font> get();
    };

    class text{
        std::weak_ptr<TTF_Font> fo;
        std::string st;
        bool empty=false;
        
        void init(const char* txt);
        
        public:
            api::image img;

            text(const char* txt, std::weak_ptr<TTF_Font> f);
            void draw();
            void setText(const char* txt);

            std::string getText();
    };

    class music{
        std::shared_ptr<SoLoud::WavStream> ptr;
        int handle=-1;
        public:
            music(const char* name, bool load=true);
            void destroy();
            void set(const char* name);
            void play(SoLoud::Soloud* soloud);
            void pause(SoLoud::Soloud* soloud);
            void resume(SoLoud::Soloud* soloud);
            void halt(SoLoud::Soloud* soloud);
            void setVol(SoLoud::Soloud* soloud, int vol);
            void setSpeed(SoLoud::Soloud* soloud, double speed);
            void skip(SoLoud::Soloud* soloud, double sk);
            double getCurrentDuration(SoLoud::Soloud* soloud);
            double getTotalDuration();
            void setDuration(SoLoud::Soloud* soloud, float time);
            int getVol();
            std::shared_ptr<SoLoud::WavStream> get();
    };
    
}