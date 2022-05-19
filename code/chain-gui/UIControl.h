//
// Created by Administrator on 2022/5/18.
//

#ifndef CHAIN_GUI_UICONTROL_H
#define CHAIN_GUI_UICONTROL_H

#include <string>
#include <set>
#include "VgBackend.h"
#include "AABB.h"

typedef unsigned int UIColor;

class WindowCollision {
public:
    void init(int w, int h);

    void insertAABB(AABBKey index, int x, int y, int w, int h);

    void updateAABB(AABBKey index, int x, int y, int w, int h);

    void removeAABB(AABBKey index);

    void queryByPoint(int x, int y, std::set<AABBKey> &processSet, std::set<AABBKey> &resultSet, bool near = false);

    void queryByAABB(int x, int y, double w, double h, std::set<AABBKey> &processSet, std::set<AABBKey> &resultSet,
                     bool near = false);

    int findByIndex(AABBKey index, std::set<AABBKey> &processSet, std::set<AABBKey> &resultSet, bool near = false);

    aabb::Tree tree;
};

class Box;

class Window;

class Collider {
public:
    int x;
    int y;
    int w;
    int h;
};

class UIEvent {
public:
//    virtual void onMouseMove(int x, int y);
//    virtual void onMouseLDown(int x, int y);
//    virtual void onMouseLUp(int x, int y);
//    virtual void onMouseRDown(int x, int y);
//    virtual void onMouseRUp(int x, int y);
//    virtual void onMouseMDown(int x, int y);
//    virtual void onMouseMUp(int x, int y);
//    virtual void onKeyDown(int Key);
//    virtual void onKeyUp(int Key);
//    virtual void onMouseWheel(int val);
//    virtual void onMouseDrag(int x, int y, int gx, int gy);
//    virtual void onResize(int newSizeW, int newSizeH);
//    virtual void onWindowResize(int newSizeW, int newSizeH);
//    virtual void onInputText(const std::string& str);
//    virtual void onFocus(bool isLoseFocus);
    bool dragable = true;
    bool isPress = false;
    int pressPosX = 0;
    int pressPosY = 0;
    bool focus = false;
};


class BaseControl : public UIEvent {
public:
    Box *parent;
    std::string title;
    int64_t id;
    Collider collider;
    int ax = 0;
    int ay = 0;
    int layer;
    int isInit;

    bool colliderOn = true;

    bool highlightShow = false;

    virtual void setPos(int x, int y);

    virtual void setSize(int w, int h);

    virtual int init(Box *parent);

    virtual int draw(int64_t tick);

    void drawBound();

    int setDirty(bool dirty);

};

struct Cmp {
    bool operator()(const BaseControl *left, const BaseControl *right) const {
        return cmp(*left, *right);
    }

    static bool cmp(const BaseControl &left, const BaseControl &right) {
        if (right.id == left.id)
            return false;
        if (right.layer != left.layer)
            return right.layer > left.layer;
        return right.id > left.id;
    }
};

class Box : public BaseControl {
public:
    virtual int init(Box *parent, int x = 0, int y = 0, int w = 800, int h = 800);

    static Box *create(Box *box, int x = 0, int y = 0, int w = 800, int h = 800);

    int draw(int64_t tick) override;

    int setColor(UIColor color);

    int resize(int w, int h);

    std::set<BaseControl *, Cmp> childControlMap;
    UIColor color = 0;
};

class Window : public Box {
public:
    static Window *create(Box *parent, const std::string &title, int x, int y, int w = 800, int h = 800);

    int init(Box *parent, int x = 0, int y = 0, int w = 800, int h = 800) override;

    BaseControl *getWidgetByPoint(int mouseX, int mouseY);

    int draw(int64_t tick);

    void onMouseMove(int x, int y);

    void onMouseLDown(int x, int y);

    void onMouseLUp(int x, int y);

    void onMouseDrag(int x, int y, int gx, int gy);

    int resize(int x, int y);

    void setCurControl(BaseControl *newControl);

    int isInit;
    int64_t id;

    UIColor color = 0;

};


#endif //CHAIN_GUI_UICONTROL_H
