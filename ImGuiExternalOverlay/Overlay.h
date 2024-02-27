#ifndef OVERLAY_H
#define OVERLAY_H

#include "Includes.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum class WindowType : int
{
    NewWindow,
    AttachWindow
};

class Overlay
{
private:
    static ID3D11Device* pd3dDevice;
    static ID3D11DeviceContext* pd3dDeviceContext;
    static IDXGISwapChain* pSwapChain;
    static ID3D11RenderTargetView* pMainRenderTargetView;
    static bool bInit;
    static HWND hTargetWindow;
    static HWND hSelfWindow;
    static WNDCLASSEX wc;
    static RECT rcLastMove;
    static RECT rcClient;
    static WindowType Type;

    static bool CreateDeviceD3D(HWND hWnd);
    static void CleanupDeviceD3D();
    static void CreateRenderTarget();
    static void CleanupRenderTarget();
    static void MoveWindow();
    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    static BOOL bRunning;
    static ImFont* Font;
    static BOOL NewInit(std::string WindowName, std::string ClassName);
    static BOOL AttachInit(HWND Target, std::string WindowName, std::string ClassName);
    static BOOL CreateImgui();
    static void Render(std::function<void()> CallBack, bool& isMenuShow);
    static void Destroy();
    static BOOL GetWindowStatus();

    static int Width, Height;
    static ImVec4 CleanColor;
};

#endif
