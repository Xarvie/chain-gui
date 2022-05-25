//
// Created by Administrator on 2022/5/18.
//

#ifndef CHAIN_GUI_CGWINDOWBACKENDAPI_H
#define CHAIN_GUI_CGWINDOWBACKENDAPI_H

#include <string>

const int ACTION_DOWN = 0;
const int ACTION_UP = 1;
const int ACTION_MOVE = 2;
const int ACTION_REPEAT_DOWN = 3;
const int ACTION_WHEEL = 4;
const int BUTTON_LEFT = 5;
const int BUTTON_MID = 6;
const int BUTTON_RIGHT = 7;



class CGWindowBackendAPI {
public:
    virtual void init() = 0;
    virtual void imeWindowInit() = 0;
    virtual void setClipboardText(const char* str) = 0;
    virtual bool hasClipboardText() = 0;
    virtual std::string getClipboardText() = 0;
    virtual void pollEvent(void * event) = 0;


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



#endif //CHAIN_GUI_CGWINDOWBACKENDAPI_H
