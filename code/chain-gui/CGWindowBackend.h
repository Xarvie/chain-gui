//
// Created by Administrator on 2022/5/19.
//

#ifndef CHAIN_GUI_CGWINDOWBACKEND_H
#define CHAIN_GUI_CGWINDOWBACKEND_H

#if defined(SDL_BACKEND)
#include "CGWindowBackendSDL.h"
#endif

class CGWindowBackend :public _CGWindowBackend{
public:
    virtual void evFinger(int action, int fingerID, float x, float y, float z);
    virtual void evQuit();
    virtual void evKey(int action, int key);
    virtual void evMouse(int action, int button, int x, int y);
    virtual void evInput(const std::string &str);
    virtual void evWindowResize(int w, int h);
};
extern CGWindowBackend* windowMgr;
#endif //CHAIN_GUI_CGWINDOWBACKEND_H
