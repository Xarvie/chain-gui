//
// Created by Administrator on 2022/5/18.
//

#ifndef CHAIN_GUI_CHAINGUI_H
#define CHAIN_GUI_CHAINGUI_H
#include "UIControl.h"
#include "VgBackend.h"

#include <set>
#include <unordered_map>


class ChainGui {
public:
    void init(int w, int h);
    static ChainGui * get();
    void pollEvent(void * event);
    unsigned char* draw();
    void setCanvasSize(int w, int h);
    UIFont* getFont(const std::string str, int fontSize);

    std::set<UIWindow*, Cmp> rootWindows;

    WindowCollision wc;
    static int64_t timeNow();
    BaseControl * getWidgetByPoint(int mouseX, int mouseY);
    bool dirty = true;
    int64_t startTick = 0;
    bool drawBound = true;
    VGBackend canvas;
    int w = 0;
    int h = 0;
    std::unordered_map<int64_t, UIFont> fontMap;
    int64_t defaultFontHash = 0;
    int64_t defaultFontSize = 0;
    UIRectF clipRect;
    UIFont* defaultFont = nullptr;
    UIEvent ev;
    UIWindow* curWindow = nullptr;
    BaseControl* curControl = nullptr;
};


#endif //CHAIN_GUI_CHAINGUI_H
