//
// Created by Administrator on 2022/5/18.
//

#ifndef CHAIN_GUI_UICONTROL_H
#define CHAIN_GUI_UICONTROL_H

#include <string>
#include <set>
#include "VgBackend.h"
#include "AABB.h"
#include "TextLayout.h"
#include "CGWindowBackend.h"
typedef unsigned int UIColor;

struct UIRectI {
    int l;
    int t;
    int r;
    int b;
};

struct UIRectF {
    double l;
    double t;
    double r;
    double b;
};


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

class Collider {
public:
    int x;
    int y;
    int w;
    int h;
};

class UIImage{
public:
    Image image;
};


class UIEvent {
public:
    UIEvent(){};
    virtual ~UIEvent(){};

        virtual void evFinger(int action, int fingerID, float x, float y, float z);
        virtual void evQuit();
        virtual void evKey(int action, int key);
        virtual void evMouse(int action, int button, int x, int y);
        virtual void evInput(const std::string &str);
        virtual void evWindowResize(int w, int h);

};

class UIControlEvent {
public:
    virtual void onMouseMove(int x, int y){};
    virtual void onMouseLDown(int x, int y){};
    virtual void onMouseLUp(int x, int y){};
    virtual void onMouseRDown(int x, int y){};
    virtual void onMouseRUp(int x, int y){};
    virtual void onMouseMDown(int x, int y){};
    virtual void onMouseMUp(int x, int y){};
    virtual void onKeyDown(int Key, bool isRepeat){};
    virtual void onKeyUp(int Key){};
    virtual void onMouseWheel(int x, int y){};
    virtual void onMouseDrag(int x, int y, int gx, int gy){};
    virtual void onResize(int newSizeW, int newSizeH){};
    virtual void onWindowResize(int newSizeW, int newSizeH){};
    virtual void onInputText(const std::string& str){};
    virtual void onFocus(bool focus_){
        this->focus = focus_;
    };
    bool dragable = true;
    bool isPress = false;
    int pressPosX = 0;
    int pressPosY = 0;
    bool focus = false;
};
enum ControlState{
    CSNormal,
    CSDisable,
    CSHover,
    CSPress
};

class BaseControl : public UIControlEvent {
public:

    virtual void setPos(int x, int y);

    virtual void setSize(int w, int h);

    virtual void calcClipRect();

    virtual void calcColliderRect();

    virtual int init(Box *parent);

    virtual int draw(int64_t tick);

    virtual int setImage(int idx, const std::string &file);

    void drawBound();

    int setDirty(bool dirty);

public:
    Box *parent;
    std::string title;
    int64_t id = 0;
    Collider collider;
    double ax = 0.0;
    double ay = 0.0;
    int layer = 0;
    int isInit = 0;
    bool drawBound_ = false;
    UIColor colorBound = 0xFFFF0000;
    bool colliderOn = true;
    bool highlightShow = false;
    UIRectF clipRect;
    bool disable = false;
    bool visible = true;
    std::vector<UIImage> images = std::vector<UIImage>(6);
    int imgIDX = ControlState::CSNormal;

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

struct WindowCmp {
    bool operator()(const BaseControl *left, const BaseControl *right) const {
        return cmp(*left, *right);
    }

    static bool cmp(const BaseControl &left, const BaseControl &right) {
        if (right.id == left.id)
            return false;
        if (right.layer != left.layer)
            return right.layer < left.layer;
        return right.id < left.id;
    }
};
class Box : public BaseControl {
public:
    virtual int init(Box *parent, int x = 0, int y = 0, int w = 800, int h = 800);

    static Box *create(Box *box, int x = 0, int y = 0, int w = 800, int h = 800);

    int draw(int64_t tick) override;
    virtual void calcColliderRect();
    int resize(int w, int h);

    std::set<BaseControl *, Cmp> childControlMap;
};

class UIWindow : public Box {
public:
    static UIWindow *create(Box *parent, const std::string &title, int x, int y, int w = 800, int h = 800);

    int init(Box *parent, int x = 0, int y = 0, int w = 800, int h = 800) override;

    int draw(int64_t tick);

    virtual void onMouseMove(int x, int y);
    virtual void onMouseLDown(int x, int y);
    virtual void onMouseLUp(int x, int y);
    virtual void onMouseRDown(int x, int y);
    virtual void onMouseRUp(int x, int y);
    virtual void onMouseMDown(int x, int y);
    virtual void onMouseMUp(int x, int y);
    virtual void onKeyDown(int Key, bool isRepeat);
    virtual void onKeyUp(int Key);
    virtual void onMouseWheel(int x, int y);
    virtual void onMouseDrag(int x, int y, int gx, int gy);
    virtual void onInputText(const std::string& str);
    virtual void onFocus(bool focus_);
    virtual void onWindowResize(int newSizeW, int newSizeH);

    int resize(int x, int y);

    void setCurControl(BaseControl *newControl);

    int isInit;
    int64_t id;

    UIColor color = 0;

    bool mouseDragWindow = false;
    double mouseDragWindowX = 0.0;
    double mouseDragWindowY = 0.0;

    double OldWindowX = 0.0;
    double OldWindowY = 0.0;
};
#define TBOX
#if defined(TBOX)
class EditBox : public BaseControl {
public:
    EditBox();

    virtual ~EditBox();

    static EditBox *create(Box *parent, const std::string &text, int x, int y, int w, int h);
    virtual void init(Box *parent, const std::string &text, int x, int y, int w, int h);
    int destroy();

    int draw(int64_t tick);

    int setFont(const std::string &fontName, float fontSize);

    virtual void onMouseMove(int x, int y);
    virtual void onMouseLDown(int x, int y);
    virtual void onMouseLUp(int x, int y);
    virtual void onMouseRDown(int x, int y);
    virtual void onMouseRUp(int x, int y);
    virtual void onMouseMDown(int x, int y);
    virtual void onMouseMUp(int x, int y);
    virtual void onKeyDown(int Key, bool isRepeat);
    virtual void onKeyUp(int Key);
    virtual void onMouseWheel(int x, int y);
    virtual void onMouseDrag(int x, int y, int gx, int gy);
    virtual void onInputText(const std::string& str);
    virtual void onFocus(bool focus_);
    virtual void onWindowResize(int newSizeW, int newSizeH);

    virtual void setPos(int x, int y);
    virtual void setSize(int w, int h);

    std::string text;
    UIColor colorText;
    UIColor colorBkg = 0;
    UIColor colorBorder = 0;

    int state;
    UIFont* font = nullptr;

    int mouseLclickPosX = 0;
    int mouseLclickPosY = 0;
    Collider clickCollider;
    TextLayout tl;

    bool ctrl = false;
    bool alt = false;
    bool shift = false;
    bool win = false;
    bool editAble = true;


};
#endif

class UILabel : public EditBox{
public:
    static UILabel *create(Box *parent, const std::string &text, int x, int y, int w, int h);
    virtual void onMouseMove(int x, int y);
    virtual void onMouseLDown(int x, int y);
    virtual void onMouseLUp(int x, int y);
    virtual void onMouseRDown(int x, int y);
    virtual void onMouseRUp(int x, int y);
    virtual void onMouseMDown(int x, int y);
    virtual void onMouseMUp(int x, int y);
    virtual void onKeyDown(int Key, bool isRepeat);
    virtual void onKeyUp(int Key);
    virtual void onMouseWheel(int x, int y);
    virtual void onMouseDrag(int x, int y, int gx, int gy);
    virtual void onInputText(const std::string& str);
    virtual void onFocus(bool focus_);
    virtual void onWindowResize(int newSizeW, int newSizeH);
};

class UIButton : public EditBox{
public:
    static UIButton *create(Box *parent, const std::string &text, int x, int y, int w, int h);
    virtual void onMouseMove(int x, int y);
    virtual void onMouseLDown(int x, int y);
    virtual void onMouseLUp(int x, int y);
    virtual void onMouseRDown(int x, int y);
    virtual void onMouseRUp(int x, int y);
    virtual void onMouseMDown(int x, int y);
    virtual void onMouseMUp(int x, int y);
    virtual void onKeyDown(int Key, bool isRepeat);
    virtual void onKeyUp(int Key);
    virtual void onMouseWheel(int x, int y);
    virtual void onMouseDrag(int x, int y, int gx, int gy);
    virtual void onInputText(const std::string& str);
    virtual void onFocus(bool focus_);
    virtual void onWindowResize(int newSizeW, int newSizeH);
};


class UIImageBox : public EditBox{
public:
    static UIImageBox *create(Box *parent, const std::string &text, int x, int y, int w, int h);
    virtual void onMouseMove(int x, int y);
    virtual void onMouseLDown(int x, int y);
    virtual void onMouseLUp(int x, int y);
    virtual void onMouseRDown(int x, int y);
    virtual void onMouseRUp(int x, int y);
    virtual void onMouseMDown(int x, int y);
    virtual void onMouseMUp(int x, int y);
    virtual void onKeyDown(int Key, bool isRepeat);
    virtual void onKeyUp(int Key);
    virtual void onMouseWheel(int x, int y);
    virtual void onMouseDrag(int x, int y, int gx, int gy);
    virtual void onInputText(const std::string& str);
    virtual void onFocus(bool focus_);
    virtual void onWindowResize(int newSizeW, int newSizeH);


};
#endif //CHAIN_GUI_UICONTROL_H
