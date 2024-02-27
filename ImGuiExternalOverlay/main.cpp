#include "Includes.h"

bool showMenu = true;
bool blackBackground = false;


void Drawing()
{
    if (showMenu)
    {
        ImGui::Begin(u8"code by cloudhoter");

        ImGui::Text(u8"菜单显示的时候无法操作其它程序");
        ImGui::Text(u8"Ins开关菜单");

        ImGui::Checkbox(u8"黑色背景", &blackBackground);

        auto io = ImGui::GetIO();
        ImGui::Text("%.1f FPS", io.Framerate);
        ImGui::End();
    }

    // 通过更改渲染循环中清理屏幕代码中的float *ColorRGBA来更改背景颜色（也许对某些人有用（比如DMA融合器绘制），不需要的就直接4个0然后删除这份代码）
    if (blackBackground)
    {
        Overlay::CleanColor = ImVec4(0, 0, 0, 255);
    }
    else
    {
        Overlay::CleanColor = ImVec4(0, 0, 0, 0);
    }

    // 绘制需要使用GetBackgroundDrawList，不能使用windowdraw和forcedraw
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(100, 100), ImVec2(300, 300), ImColor(255, 255, 0), 0, 0);
    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(400, 400), 30.f, ImColor(255, 0, 255), 32, 5.f);
}

int main()
{
    // 新建一个全屏窗口
    //if (Overlay::NewInit("MyOverlayWindow","MyOverlayClass") == FALSE)
    //{
    //    exit(0);
    //}
    // 
    
    // 新建一个附加进程窗口
    HWND target = FindWindowA("Notepad", NULL); // 记事本
    if (Overlay::AttachInit(target, "MyOverlayWindow", "MyOverlayClass") == FALSE)
    {
        exit(0);
    }

    if (Overlay::CreateImgui() == FALSE)
    {
        exit(0);
    }

    Overlay::Render(Drawing, showMenu);
    Overlay::Destroy();

    return 0;
}
