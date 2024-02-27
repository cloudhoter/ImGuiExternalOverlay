# ImGuiExternalOverlay

这是一个使用DirectX 3D 11和ImGui 1.90制作的简易的外部Overlay类

你可以轻松的使用它创建一个Overlay窗口（可以单独创建全屏窗口，也可以附加别的进程）

### 创建全屏窗口

```c_cpp
Overlay::NewInit("MyOverlayWindow","MyOverlayClass");
```

提交WindowName和ClassName即可，注意判断返回值（BOOL）

### 创建附加窗口

```c_cpp
HWND target = FindWindowA("Notepad", NULL); // 记事本
Overlay::AttachInit(target, "MyOverlayWindow", "MyOverlayClass");
```

提交目标进程HWND，WindowName和ClassName即可，注意判断返回值（BOOL）

### 渲染函数

窗口初始化完成后，还需要初始化ImGui，随后可以调用Render函数

```c_cpp
Overlay::CreateImgui();
Overlay::Render(Drawing, showMenu);
Overlay::Destroy();
```

Render函数的函数声明为

```c_cpp
void Overlay::Render(std::function<void()> CallBack, bool& isMenuShow)
```

意味着你需要传入一个函数用于在渲染循环中调用，传入一个bool类型的代表菜单是否显示

1. 传入的函数示例
   ```c_cpp
    void Drawing()
    {
        if (showMenu)
        {
            ImGui::Begin(u8"code by cloudhoter");
    
            ImGui::Text(u8"菜单显示的时候无法操作其它程序");
            ImGui::Text(u8"Ins开关菜单");
    
            auto io = ImGui::GetIO();
            ImGui::Text("%.1f FPS", io.Framerate);
            ImGui::End();
        }
    
        // 绘制需要使用GetBackgroundDrawList，不能使用windowdraw和forcedraw
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(100, 100), ImVec2(300, 300), ImColor(255, 255, 0), 0, 0);
        ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(400, 400), 30.f, ImColor(255, 0, 255), 32, 5.f);
    }
   ```
   
   ImGui的窗口创建和绘制都需要在这个传入函数内实现，你可以自己封装
2. 菜单显示变量
   
   传入的菜单显示变量会在Render函数的循环中判断是否按下Insert按键，然后进行取反操作，你可以删除这部分代码自己编写
   
   这是判断是否按下Insert按键的代码
   ```c_cpp
   // 菜单
   if (GetAsyncKeyState(VK_INSERT) & 0x8000)
   {
       // 进行一个200ms的延迟判断，不然窗口一直闪
       static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
       auto currentTime = std::chrono::steady_clock::now();
       auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
       if (duration > std::chrono::milliseconds(200))
       {
           lastTime = currentTime;
           isMenuShow = !isMenuShow;
       }
   }
   ```
