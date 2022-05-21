# chain-GUI
### 编译:
``` sh
cd ./chain-gui
mkdir build
cd build
cmake ..
```

### TODO:   
- 设备支持：
  - 基础事件 <font color="#dd0000">40%</font> 
    - SDL后端 <font color="#dd0000">70%</font>
    - 自定义后端 <font color="#dd0000">0%</font>
  - GPU硬渲染 <font color="#dd0000">0%</font>
  - CPU软渲染 <font color="#dd0000">✓</font>
- 布局器 
  - 固定布局  <font color="#dd0000">30%</font>
  - 比例布局 <font color="#dd0000">0%</font>
  - 自动布局 <font color="#dd0000">0%</font>
- 窗口 <font color="#dd0000">70%</font>
- 按钮 <font color="#dd0000">0%</font>
- 标签 <font color="#dd0000">0%</font>
- 图片框 <font color="#dd0000">0%</font>
- 文本编辑框 
  - 预览器（View） 
    - 不自动换行 <font color="#dd0000">0%</font> 
    - 自动换行 <font color="#dd0000">70%</font>
      - 分字分行 <font color="#dd0000">90%</font>
      - ~~分词、分符分行~~
      - ~~RTL~~
  - 选取器（Selector） 
    - 单光标选取器 <font color="#dd0000">0%</font> 
      - 选取区域与光标渲染 <font color="#dd0000">0%</font> 
    - 鼠标支持 <font color="#dd0000">0%</font> 
      - 左键支持 <font color="#dd0000">0%</font> 
      - 单击 <font color="#dd0000">0%</font> 
      - 双击 <font color="#dd0000">0%</font> 
      - 三击 <font color="#dd0000">0%</font> 
      - 四击 <font color="#dd0000">0%</font> 
      - 框选 <font color="#dd0000">0%</font> 
      - 右键支持 <font color="#dd0000">0%</font> 
      - 中键支持 <font color="#dd0000">0%</font>
    - 键盘绑定 <font color="#dd0000">0%</font> 
      - 方向键 跳字 <font color="#dd0000">0%</font> 
      - alt 跳词 <font color="#dd0000">0%</font> 
      - insert 插入/替换模式切换 <font color="#dd0000">0%</font> 
      - home 跳转行首 <font color="#dd0000">0%</font> 
      - backspace/delete 向前/向后删除 <font color="#dd0000">0%</font> 
      - PgUp/PgDn 向上/向下翻页 <font color="#dd0000">0%</font> 
      - shift + 方向键 选取 <font color="#dd0000">0%</font> 
      - alt + 左右 光标回溯 <font color="#dd0000">0%</font> 
      - ...
    - 触摸屏幕支持 <font color="#dd0000">0%</font> 
    - 选取区域与光标渲染 <font color="#dd0000">0%</font> 
    - 手柄摇杆支持 <font color="#dd0000">0%</font> 
    - 多光标选取器(Beta计划) <font color="#dd0000">0%</font> 
    - ctrl+shift+alt+上下方向 多选 <font color="#dd0000">0%</font> 
    - 方向键 跳字 <font color="#dd0000">0%</font> 
    - alt 跳词 <font color="#dd0000">0%</font> 
    - insert 插入/替换模式切换 <font color="#dd0000">0%</font> 
    - home 跳转行首 <font color="#dd0000">0%</font> 
    - backspace/delete 向前/向后删除 <font color="#dd0000">0%</font> 
    - PgUp/PgDn 向上/向下翻页 <font color="#dd0000">0%</font> 
    - shift + 方向键 选取 <font color="#dd0000">0%</font> 
    - alt + 左右 光标回溯 <font color="#dd0000">0%</font> 
      - ...
  - 编辑器(edit)
    - 跳字 <font color="#dd0000">0%</font> 
    - 跳词 <font color="#dd0000">0%</font> 
    - 插入/替换模式切换 <font color="#dd0000">0%</font> 
    - 跳转行首 <font color="#dd0000">0%</font> 
    - 向前删除 <font color="#dd0000">0%</font> 
    - 反向删除 <font color="#dd0000">0%</font> 
    - 向上/向下翻页 <font color="#dd0000">0%</font> 
    - 复制/粘贴 <font color="#dd0000">0%</font> 
    - 撤销 <font color="#dd0000">0%</font> 
    - 反撤销 <font color="#dd0000">0%</font> 
    - 光标历史 <font color="#dd0000">0%</font> 
    - 光标回溯 <font color="#dd0000">0%</font> 
    - 富文本 <font color="#dd0000">0%</font> 
      - 对齐 <font color="#dd0000">0%</font> 
      - 加粗 <font color="#dd0000">0%</font> 
      - 倾斜 <font color="#dd0000">0%</font> 
      - 字号 <font color="#dd0000">0%</font> 
      - 图片 <font color="#dd0000">0%</font> 
      - 色彩 <font color="#dd0000">0%</font>