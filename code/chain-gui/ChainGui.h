//
// Created by Administrator on 2022/5/18.
//

#ifndef CHAIN_GUI_CHAINGUI_H
#define CHAIN_GUI_CHAINGUI_H
#include "CGWindowBackend.h"
#include "VgBackend.h"
#include "UIControl.h"
#include <set>

class ChainGui {
public:
    void init(int w, int h);
    static ChainGui * get();
    void pollEvent(void * event);
    unsigned char* draw();
    void setCanvasSize(int w, int h);

    std::set<Window*> rootWindows;

    WindowCollision wc;
    static int64_t timeNow();

    bool dirty = true;
    int64_t startTick = 0;
    bool drawBound = true;
    VGBackend canvas;
    int w = 0;
    int h = 0;
};


#endif //CHAIN_GUI_CHAINGUI_H
