//
// Created by Administrator on 2022/5/18.
//
#include "SDL.h"
#include "chain-gui/ChainGui.h"

int main(int argc, char *argv[]) {

    windowMgr->init();
    auto ret = SDL_InitSubSystem(SDL_INIT_VIDEO);

    auto* window = SDL_CreateWindow("sdl demo",
                                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    500, 500,
                                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);


    while(true)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            windowMgr->pollEvent(&e);
            switch (e.type) {
                case SDL_QUIT:
                    return 0;
                case SDL_FINGERMOTION:
                case SDL_FINGERDOWN:
                case SDL_FINGERUP:
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                case SDL_MOUSEWHEEL:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_TEXTINPUT:
                case SDL_WINDOWEVENT_RESIZED:
                    break;
                default:
                    break;

            }

        }
    }

    return 0;
}