#include "Includes.h"

bool showMenu = true;
bool blackBackground = false;


void Drawing()
{
    if (showMenu)
    {
        ImGui::Begin(u8"code by cloudhoter");

        ImGui::Text(u8"�˵���ʾ��ʱ���޷�������������");
        ImGui::Text(u8"Ins���ز˵�");

        ImGui::Checkbox(u8"��ɫ����", &blackBackground);

        auto io = ImGui::GetIO();
        ImGui::Text("%.1f FPS", io.Framerate);
        ImGui::End();
    }

    // ͨ��������Ⱦѭ����������Ļ�����е�float *ColorRGBA�����ı�����ɫ��Ҳ���ĳЩ�����ã�����DMA�ں������ƣ�������Ҫ�ľ�ֱ��4��0Ȼ��ɾ����ݴ��룩
    if (blackBackground)
    {
        Overlay::CleanColor = ImVec4(0, 0, 0, 255);
    }
    else
    {
        Overlay::CleanColor = ImVec4(0, 0, 0, 0);
    }

    // ������Ҫʹ��GetBackgroundDrawList������ʹ��windowdraw��forcedraw
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(100, 100), ImVec2(300, 300), ImColor(255, 255, 0), 0, 0);
    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(400, 400), 30.f, ImColor(255, 0, 255), 32, 5.f);
}

int main()
{
    // �½�һ��ȫ������
    //if (Overlay::NewInit("MyOverlayWindow","MyOverlayClass") == FALSE)
    //{
    //    exit(0);
    //}
    // 
    
    // �½�һ�����ӽ��̴���
    HWND target = FindWindowA("Notepad", NULL); // ���±�
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
