#pragma once

#include <SDL_mixer.h>
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
        void init(const char* txt);
        std::string st;
        public:
            api::image img;
            text(const char* txt, std::weak_ptr<TTF_Font> f);
            void draw();
            void setText(const char* txt);
            std::string getText();
    };
    class sound{
        std::shared_ptr<Mix_Chunk> ptr;
        public:
            sound(const char* name);
            void play();
            void setVol(Uint8 vol);
            std::shared_ptr<Mix_Chunk> get();
    };
    class music{
        std::shared_ptr<Mix_Music> ptr;
        public:
            music(const char* name);
            void play(bool loops=true);
            void pause();
            void resume();
            void halt();
            void setVol(Uint8 vol);
            std::shared_ptr<Mix_Music> get();
    };
    
}