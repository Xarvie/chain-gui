//
// Created by Administrator on 2022/5/18.
//

#include "ChainGui.h"
#include "CGWindowBackend.h"
;
void ChainGui::init(int w, int h) {
    windowMgr->init();
    canvas.init(w, h);
    wc.init(10000, 10000);
    startTick = ChainGui::timeNow();
    this->w = w;
    this->h = h;
}

void ChainGui::pollEvent(void * event) {
    windowMgr->pollEvent(event);
}

unsigned char* ChainGui::draw(){
    auto curTick = ChainGui::timeNow();
    canvas.drawBegin(NULL);
    canvas.setFillStyle(0x00FFFFFF);
    canvas.fillAll();

    for(auto window:this->rootWindows){
        window->draw(curTick);
    }

    canvas.drawEnd();
    return canvas.getPixels();
}

void ChainGui::setCanvasSize(int w, int h){
    this->w = w;
    this->h = h;
    canvas.setCanvasSize(w, h);
}

ChainGui *ChainGui::get() {
    static ChainGui gui;
    return &gui;
}

int64_t ChainGui::timeNow() {
#ifdef _WIN32
        _timeb timebuffer;
        _ftime(&timebuffer);
        int64_t ret = timebuffer.time;
        ret = ret * 1000 + timebuffer.millitm;
        return ret;
#else
        timeval tv;
    ::gettimeofday(&tv, 0);
    int64_t ret = tv.tv_sec;
    return ret * 1000 + tv.tv_usec / 1000;
#endif
}
