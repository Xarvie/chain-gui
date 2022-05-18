//
// Created by Administrator on 2022/5/18.
//
#include "SDL.h"
#include "chain-gui/ChainGui.h"
#include <iostream>

int main(int argc, char *argv[]) {
    windowMgr->init();
    auto ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if(ret != 0)
        std::cout << "create Window Error." << SDL_GetError() << std::endl;
    auto* window = SDL_CreateWindow(
            "sdl demo",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            500, 500,
            SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(window == NULL)
        std::cout << "create Window Error." << std::endl;

    auto renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

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
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    break;
                default:
                    break;

            }

        }
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    return 0;
}