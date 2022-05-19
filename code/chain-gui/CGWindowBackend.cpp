//
// Created by Administrator on 2022/5/20.
//

#include "ChainGui.h"
#include "CGWindowBackend.h"
CGWindowBackend* windowMgr = new CGWindowBackend;

void CGWindowBackend::evFinger(int action, int fingerID, float x, float y, float z){

}

void CGWindowBackend::evQuit(){

}

void CGWindowBackend::evKey(int action, int key){

}

void CGWindowBackend::evMouse(int action, int button, int x, int y){

}

void CGWindowBackend::evInput(const std::string &str){

}

void CGWindowBackend::evWindowResize(int w, int h){
    auto gui = ChainGui::get();
    gui->setCanvasSize(w, h);
}
