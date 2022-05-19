//
// Created by Administrator on 2022/5/19.
//

#ifndef CHAIN_GUI_UICANVAS_H
#define CHAIN_GUI_UICANVAS_H
#include "VgBackend.h"

class UICanvas {
public:
    void init(int w, int h);
    void resize(int w, int h);
    VGBackend _painter;
};


#endif //CHAIN_GUI_UICANVAS_H
