//
// Created by Administrator on 2022/5/18.
//

#ifndef CHAIN_GUI_CGWINDOWBACKENDSDL_H
#define CHAIN_GUI_CGWINDOWBACKENDSDL_H
#include "CGWindowBackendAPI.h"
#include <string>

class CGWindowBackendSDL : public CGWindowBackendAPI{
public:
    void init() override;
    void imeWindowInit() override;
    void setClipboardText(const char* str) override;
    bool hasClipboardText() override;
    std::string getClipboardText() override;
    void pollEvent(void * event) override;

    virtual void evFinger(int action, int fingerID, float x, float y, float z){

    }
    virtual void evQuit(){

    }
    virtual void evKey(int action, int key){

    }
    virtual void evMouse(int action, int button, int x, int y){

    }
    virtual void evInput(const std::string &str){

    }
    virtual void evWindowResize(int w, int h){

    }
};

typedef CGWindowBackendSDL CGWindowBackend;
extern CGWindowBackend* windowMgr;
#endif //CHAIN_GUI_CGWINDOWBACKENDAPI_H
