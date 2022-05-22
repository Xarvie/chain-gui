//
// Created by Administrator on 2022/5/20.
//

#include "ChainGui.h"
#include "CGWindowBackend.h"
CGWindowBackend* windowMgr = new CGWindowBackend;

void CGWindowBackend::evFinger(int action, int fingerID, float x, float y, float z){
    ChainGui::get()->ev.evFinger(action, fingerID, x, y, z);
}

void CGWindowBackend::evQuit(){
    ChainGui::get()->ev.evQuit();
}

void CGWindowBackend::evKey(int action, int key){
    ChainGui::get()->ev.evKey(action, key);
}

void CGWindowBackend::evMouse(int action, int button, int x, int y){
    ChainGui::get()->ev.evMouse(action, button, x, y);
}

void CGWindowBackend::evInput(const std::string &str){
    ChainGui::get()->ev.evInput(str);
}

void CGWindowBackend::evWindowResize(int w, int h){
    auto gui = ChainGui::get();
    ChainGui::get()->ev.evWindowResize(w, h);
}
