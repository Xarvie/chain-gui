//
// Created by Administrator on 2022/5/18.
//
#include "SDL.h"
#include "chain-gui/ChainGui.h"
#include <iostream>
#include <vector>
#include <mutex>

int w = 2560; int h = 1440;
int main(int argc, char *argv[]) {
    auto gui = ChainGui::get();

    gui->init(w, h);
    auto ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if(ret != 0)
        std::cout << "create Window Error." << SDL_GetError() << std::endl;
    auto* window = SDL_CreateWindow(
            "sdl demo",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            w, h,
             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(window == NULL)
        std::cout << "create Window Error." << std::endl;

//    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Texture* texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            w, h
    );


    Window win;
    win.create(NULL, "window1",5, 5,w-10, h-10);
    while(true)
    {
        SDL_Event e;
        SDL_RenderClear(renderer);
        unsigned char* pixels = gui->draw();
        SDL_UpdateTexture(
                texture,
                NULL,
                pixels,
                w * 4
        );
        SDL_RenderCopy( renderer, texture, NULL, NULL );
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e)) {
            gui->pollEvent(&e);
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
                case SDL_WINDOWEVENT:{
                    switch (e.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            w =  e.window.data1;
                            h = e.window.data2;


                            SDL_DestroyTexture(texture);
                            texture = SDL_CreateTexture(
                                    renderer,
                                    SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    w, h
                            );
                            break;
                        default:
                            break;
                    }

                }

                default:
                    break;

            }

        }

    }

    return 0;
}