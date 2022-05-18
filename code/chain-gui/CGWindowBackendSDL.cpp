//
// Created by Administrator on 2022/5/18.
//

#include <string>
#include "CGWindowBackendSDL.h"

CGWindowBackend* windowMgr = new CGWindowBackend;

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
            float x=-1.0f;
            float y=-1.0f;
            float z=-1.0f;
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


/*//
// Created by Administrator on 2022/5/18.
//

#include <string>
#include "CGWindowBackend.h"
#include "SDLHead.h"
CGWindowBackend* windowMgr = new CGWindowBackend;

void CGWindowBackendSDL::init() {
    CGWindowBackendIMPL::init();
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
void evFinger(){}
void evQuit(){}
void evKey(){}

void CGWindowBackendSDL::pollEvent(void * event) {
    SDL_Event & ev = *(SDL_Event*)event;
    switch (e.type) {
        case SDL_FINGERMOTION:
        case SDL_FINGERDOWN:
        case SDL_FINGERUP: {
            evFinger();
            //std::cout <<"finger" << __LINE__ << std::endl;
            break;
        }
        case SDL_QUIT:
            int key;
            int keyAction = 0;//down , up,
            evQuit(key, type);
            return 0;

        case SDL_KEYDOWN: {
            auto windowID = e.motion.windowID;
            auto &window = SkiUIManager::get()->windowMap[windowID];

            if (!window->curControl)
                break;
            int key = e.key.keysym.sym;
            window->curControl->onKeyDown(key);
            break;
        }
        case SDL_KEYUP: {
            auto windowID = e.motion.windowID;
            auto &window = SkiUIManager::get()->windowMap[windowID];

            if (!window->curControl)
                break;
            int key = e.key.keysym.sym;
            window->curControl->onKeyUp(key);
//                this->key_callback((DeviceWindow *) this->_window->rawPtr,
//                                   e.key.keysym.sym,
//                                   e.key.keysym.scancode,
//                                   e.type,
//                                   e.key.keysym.mod
//                );
            //std::cout <<"finger" << __LINE__ << std::endl;
            break;
        }
        case SDL_MOUSEWHEEL: {
            int button;
            int keyAction = 0;//down , up, move, wheel up wheel down
            int x = 0;
            int y = 0;
            evMouse(button, keyAction, x, y)
            auto windowID = e.motion.windowID;
            auto &window = SkiUIManager::get()->windowMap[windowID];

            if (!window->curControl)
                break;


            window->curControl->onMouseWheel(e.wheel.y);
//                    this->tl.pageMovePixel(-1.0);


            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
//                this->mouse_button_callback((DeviceWindow *) this->_window->rawPtr,e.button.button ,e.type, 0);
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;

            auto windowID = e.motion.windowID;
            auto &window = SkiUIManager::get()->windowMap[windowID];

            SDL_CaptureMouse(SDL_TRUE);
            if (gEventor == NULL) {
                gEventor = (UIEvent *) window->getWidgetByPoint(mouseX, mouseY);
                if (!gEventor) {
                    gEventor = (UIEvent *) window;
                }
            }


            switch (e.button.button) {
                case SDL_BUTTON_LEFT: {
                    gEventor->isPress = true;
                    SDL_GetGlobalMouseState(&gEventor->pressPosX, &gEventor->pressPosY);
                    gEventor->onMouseLDown(mouseX, mouseY);
                    break;
                }
//                    case SDL_BUTTON_RIGHT: {
//                        if (!topLayerBaseControl)
//                            window->onMouseLDown(mouseX, mouseY);
//                        else
//                            topLayerBaseControl->onMouseRDown(mouseX, mouseY);
//                        break;
//                    }
//                    case SDL_BUTTON_MIDDLE: {
//                        if (!topLayerBaseControl)
//                            window->onMouseLDown(mouseX, mouseY);
//                        else
//                            topLayerBaseControl->onMouseMDown(mouseX, mouseY);
//                        break;
//                    }
            }

            break;
        }
        case SDL_MOUSEBUTTONUP: {
            //                this->mouse_button_callback((DeviceWindow *) this->_window->rawPtr,e.button.button ,e.type, 0);
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;

            auto windowID = e.motion.windowID;
            auto &window = SkiUIManager::get()->windowMap[windowID];
            SDL_CaptureMouse(SDL_FALSE);
            if (gEventor == NULL) {
                gEventor = (UIEvent *) window->getWidgetByPoint(mouseX, mouseY);
                if (!gEventor) {
                    gEventor = (UIEvent *) window;
                }
            }
            switch (e.button.button) {
                case SDL_BUTTON_LEFT: {
                    gEventor->isPress = false;
                    gEventor->onMouseLUp(mouseX, mouseY);
                    gEventor = NULL;
                    break;
                }
                case SDL_BUTTON_RIGHT: {
//                        if (!topLayerBaseControl)
//                            window->onMouseRUp(mouseX, mouseY);
//                        else
//                            topLayerBaseControl->onMouseRUp(mouseX, mouseY);
//                        break;
                }
                case SDL_BUTTON_MIDDLE: {
//                        if (!topLayerBaseControl)
//                            window->onMouseMUp(mouseX, mouseY);
//                        else
//                            topLayerBaseControl->onMouseMUp(mouseX, mouseY);
//                        break;
                }
            }


            break;
        }

        case SDL_MOUSEMOTION: {
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;

            auto windowID = e.motion.windowID;
            auto &window = SkiUIManager::get()->windowMap[windowID];

            if (gEventor == NULL) {
                auto *eventor = (UIEvent *) window->getWidgetByPoint(mouseX, mouseY);
                if (!eventor) {
                    eventor = (UIEvent *) window;
                }
                eventor->onMouseMove(mouseX, mouseY);
            } else {
                int newX = 0;
                int newY = 0;
                SDL_GetGlobalMouseState(&newX, &newY);
                gEventor->onMouseDrag(mouseX, mouseY, newX, newY);
            }

//                if (!topLayerBaseControl)
//                    window->onMouseMove(mouseX, mouseY);
//                else {
//                    topLayerBaseControl->onMouseMove(mouseX, mouseY);
//                    topLayerBaseControl->highlight(true);
//                }


            break;
        }
        case SDL_TEXTINPUT:
        {
            std::string inputStr;
            evInput(inputStr);
            auto windowID = e.motion.windowID;
            auto &window = SkiUIManager::get()->windowMap[windowID];

            if (!window->curControl)
                break;

            window->curControl->onInputText(e.text.text);
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
*/