//
// Created by Administrator on 2022/5/18.
//

#include <string>
#include "CGWindowBackendSDL.h"

void CGWindowBackendSDL::init() {

}

void CGWindowBackendSDL::imeWindowInit() {
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
}

void CGWindowBackendSDL::setClipboardText(const char *str) {
    SDL_SetClipboardText(str);
}

bool CGWindowBackendSDL::hasClipboardText() {
    return SDL_HasClipboardText();
}

std::string CGWindowBackendSDL::getClipboardText(){
    std::string ret = SDL_GetClipboardText();
    return ret;
}


void CGWindowBackendSDL::pollEvent(void * event) {
    SDL_Event & ev = *(SDL_Event*)event;
    switch (ev.type) {
        case SDL_FINGERMOTION:
        case SDL_FINGERDOWN:
        case SDL_FINGERUP: {
            float x=0;
            float y=0;
            float z=0;
            int action = -1;
            int fingerID = ev.tfinger.fingerId;
            if(ev.type == SDL_FINGERMOTION){
                action = ACTION_MOVE;
                x=ev.tfinger.x;//TODO DPI?
                y=ev.tfinger.y;
                z=ev.tfinger.pressure;
            }else if(ev.type == SDL_FINGERDOWN){
                action = ACTION_DOWN;
            }else if(ev.type == SDL_FINGERUP){
                action = ACTION_UP;
            }

            evFinger(action, fingerID, x, y, z);
            break;
        }
        case SDL_QUIT:
        {
            evQuit();
        }

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            int key = ev.key.keysym.sym;
            int action = ACTION_DOWN;
            if(ev.type == SDL_KEYUP)
                action = ACTION_UP;
            else if(action == ACTION_DOWN && ev.key.repeat){
                action = ACTION_REPEAT_DOWN;
            }
            evKey(key, action);
            break;
        }
        case SDL_MOUSEMOTION:
        case SDL_MOUSEWHEEL:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:{
            int button = -1;
            int x = 0; int y = 0;
            int action = 0;//down, up, move, wheel up, wheel down



            if(ev.type == SDL_MOUSEBUTTONUP){
                action = ACTION_DOWN;
                x = ev.button.x;
                y = ev.button.y;
                if(ev.button.button == SDL_BUTTON_LEFT){
                    button = BUTTON_LEFT;
                }else if(ev.button.button == SDL_BUTTON_MIDDLE){
                    button = BUTTON_MID;
                }else if(ev.button.button == SDL_BUTTON_RIGHT){
                    button = BUTTON_RIGHT;
                }

            }else if(ev.type == SDL_MOUSEBUTTONDOWN){
                action = ACTION_DOWN;
                x = ev.button.x;
                y = ev.button.y;
                if(ev.button.button == SDL_BUTTON_LEFT){
                    button = BUTTON_LEFT;
                }else if(ev.button.button == SDL_BUTTON_MIDDLE){
                    button = BUTTON_MID;
                }else if(ev.button.button == SDL_BUTTON_RIGHT){
                    button = BUTTON_RIGHT;
                }
            }else if(ev.type == SDL_MOUSEWHEEL){
                x = ev.wheel.x;
                y = ev.wheel.y;
            }else if(ev.type == SDL_MOUSEMOTION){
                action = ACTION_MOVE;
                x = ev.motion.x;
                y = ev.motion.y;
            }
            evMouse(action, button, x, y);
            break;
        }

        case SDL_TEXTINPUT:
        {
            std::string inputStr = ev.text.text;
            evInput(inputStr);
            break;
        }
        case SDL_WINDOWEVENT:  {
            switch (ev.window.event)  {
                case SDL_WINDOWEVENT_SIZE_CHANGED:  {
                    int width = ev.window.data1;
                    int height = ev.window.data2;
                    evWindowResize(width, height);

                    break;
                }
            }
            break;
        }


    }

}