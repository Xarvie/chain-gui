//
// Created by Administrator on 2022/5/18.
//
#include "SDL.h"
#include "chain-gui/ChainGui.h"
#include <iostream>
#include <vector>
#include <mutex>

int w = 1920; int h = 1080;
int main(int argc, char *argv[]) {
    auto gui = ChainGui::get();

    gui->init(w, h);
    auto ret = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if(ret != 0)
        std::cout << "create UIWindow Error." << SDL_GetError() << std::endl;
    auto* window = SDL_CreateWindow(
            "sdl demo",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            w, h,
             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(window == NULL)
        std::cout << "create UIWindow Error." << std::endl;


//    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
//    glxinfo | grep "OpenGL version" check GPU support
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Texture* texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            w, h
    );

    auto win1 = UIWindow::create(NULL, "window1", 50, 35, 800, 600);
    auto edit1 = EditBox::create(win1,
                                 "I'm the star in the northern sky, I never stayed anywhere\n"
                                 "我是北天一颗星辰，天涯海角无处停留\n"
                                 "Я звезда северного неба, я нигде не останавливался\n"
                                 "我是北天一顆星辰，天涯海角無處停留\n"
                                 "Soy la estrella en el cielo del norte, nunca me quedé en ningún lado\n"
                                 "私は北の空の星です、私はどこにも滞在しませんでした\n"
                                 "나는 북쪽 하늘의 별, 어디에도 머물지 않았어\n"
                                 "Je suis l'étoile dans le ciel du nord, je ne suis jamais resté nulle part\n"
                                 "Eu sou a estrela no céu do norte, nunca fiquei em lugar nenhum\n"
                                 "Ngiyinkanyezi esibhakabhakeni sasenyakatho, angizange ngihlale ndawo\n"
                                 "Jaz sem zvezda na severnem nebu, nikoli nisem ostal nikjer\n",
                                 30,30, 300, 360);

    auto label1 = UILabel::create(win1,
                                  "label1",
                                  50+300,30, 150, 40);

    auto button1 = UIButton::create(win1,
                                  "button1",
                                    50+300,20+50, 150, 40);

    auto imageBox1 = UIImageBox::create(win1,
                                    "imageBox1",
                                        50+300,20+50+50, 140, 110);
    imageBox1->setImage(0, "../Res/image1.png");
    imageBox1->setImage(1, "../Res/image1.png");
    imageBox1->setImage(2, "../Res/image1.png");
    imageBox1->setImage(3, "../Res/image1.png");
    {
        auto win2 = UIWindow::create(NULL, "window2", 500, 500, 250, 250);
        auto label1 = UILabel::create(win2,
                                      "label1",
                                      30,30, 150, 40);
    }
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