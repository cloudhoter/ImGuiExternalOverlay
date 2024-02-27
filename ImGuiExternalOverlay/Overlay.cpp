#include "Overlay.h"


ID3D11Device* Overlay::pd3dDevice = nullptr;
ID3D11DeviceContext* Overlay::pd3dDeviceContext = nullptr;
IDXGISwapChain* Overlay::pSwapChain = nullptr;
ID3D11RenderTargetView* Overlay::pMainRenderTargetView = nullptr;
bool Overlay::bInit = false;
HWND Overlay::hTargetWindow = nullptr;
HWND Overlay::hSelfWindow = nullptr;
WNDCLASSEX Overlay::wc = {};
RECT Overlay::rcLastMove = {};
RECT Overlay::rcClient = {};
BOOL Overlay::bRunning = TRUE;
ImFont* Overlay::Font = nullptr;
int Overlay::Width = 0;
int Overlay::Height = 0;
ImVec4 Overlay::CleanColor = { 0,0,0,0 };
WindowType Overlay::Type = WindowType::NewWindow;

// 创建D3D设备
bool Overlay::CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    const UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

// 创建渲染目标
void Overlay::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (pBackBuffer != nullptr)
    {
        pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pMainRenderTargetView);
        pBackBuffer->Release();
    }
}

// 清理渲染目标
void Overlay::CleanupRenderTarget()
{
    if (pMainRenderTargetView)
    {
        pMainRenderTargetView->Release();
        pMainRenderTargetView = nullptr;
    }
}

void Overlay::MoveWindow()
{
    if (hTargetWindow == nullptr)
        return;

    // 移动窗口, 缩放大小
    WINDOWINFO pwi{};
    GetWindowInfo(hTargetWindow, &pwi);

    rcClient = pwi.rcClient;

    Width = rcClient.right - rcClient.left;
    Height = rcClient.bottom - rcClient.top;

    RECT moveTarget = { rcClient.left , rcClient.top , rcClient.right ,rcClient.bottom };

    if (rcLastMove != moveTarget) {
        ::MoveWindow(hSelfWindow, moveTarget.left, moveTarget.top, Width, Height, FALSE);
        rcLastMove = moveTarget;
    }

    // 移动窗口到游戏窗口前
    HWND prevHwnd = GetWindow(hTargetWindow, GW_HWNDPREV);
    if (hSelfWindow != prevHwnd) {
        SetWindowPos(hSelfWindow, prevHwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
}

// 清理D3D设备
void Overlay::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (pSwapChain)
    {
        pSwapChain->Release();
        pSwapChain = nullptr;
    }

    if (pd3dDeviceContext)
    {
        pd3dDeviceContext->Release();
        pd3dDeviceContext = nullptr;
    }

    if (pd3dDevice)
    {
        pd3dDevice->Release();
        pd3dDevice = nullptr;
    }
}

// 窗口过程函数
LRESULT WINAPI Overlay::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;

    default:
        break;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

// 判断窗口是否在前台
BOOL Overlay::GetWindowStatus()
{
    if (hTargetWindow == GetForegroundWindow()) {
        return TRUE;
    }

    if (hTargetWindow == GetActiveWindow()) {
        return TRUE;
    }

    if (GetActiveWindow() == GetForegroundWindow()) {
        return TRUE;
    }

    return FALSE;

}

BOOL Overlay::NewInit(std::string WindowName, std::string ClassName)
{
    ImGui_ImplWin32_EnableDpiAwareness();

    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = _T(ClassName.c_str());
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW;

    Width = GetSystemMetrics(SM_CXSCREEN);
    Height = GetSystemMetrics(SM_CYSCREEN);

    ::RegisterClassExA(&wc);
    hSelfWindow = ::CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName, _T(WindowName.c_str()), WS_POPUP,
        0, 0, Width, Height, nullptr, nullptr, wc.hInstance, nullptr);

    SetWindowLongPtr(hSelfWindow, GWL_EXSTYLE, GetWindowLongPtr(hSelfWindow, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hSelfWindow, 0, 255, LWA_ALPHA);
    const MARGINS margin = { -1, 0, 0, 0 };
    DwmExtendFrameIntoClientArea(hSelfWindow, &margin);

    if (!CreateDeviceD3D(hSelfWindow))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return FALSE;
    }

    ::ShowWindow(hSelfWindow, SW_SHOWDEFAULT);
    ::UpdateWindow(hSelfWindow);

    Type = WindowType::NewWindow;

    return TRUE;
}

BOOL Overlay::AttachInit(HWND Target, std::string WindowName, std::string ClassName)
{
    if (Target == NULL) return FALSE;
    hTargetWindow = Target;
    SetForegroundWindow(hTargetWindow);

    ImGui_ImplWin32_EnableDpiAwareness();

    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = _T(ClassName.c_str());
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW;

    ::RegisterClassExA(&wc);
    hSelfWindow = ::CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName, _T(WindowName.c_str()), WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

    SetWindowLongPtr(hSelfWindow, GWL_EXSTYLE, GetWindowLongPtr(hSelfWindow, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hSelfWindow, 0, 255, LWA_ALPHA);
    const MARGINS margin = { -1, 0, 0, 0 };
    if (DwmExtendFrameIntoClientArea(hSelfWindow, &margin) != S_OK)
        return FALSE;

    if (!CreateDeviceD3D(hSelfWindow))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return FALSE;
    }

    ::ShowWindow(hSelfWindow, SW_SHOWDEFAULT);
    ::UpdateWindow(hSelfWindow);

    Type = WindowType::AttachWindow;

    MoveWindow();

    return TRUE;
}

BOOL Overlay::CreateImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 关闭ini文件保存
    ImGui::GetIO().IniFilename = nullptr;
    // 加载字体
    {
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyh.ttc", 17.0f, &fontConfig, io.Fonts->GetGlyphRangesChineseFull()); // 微软雅黑
    }

    if (!ImGui_ImplWin32_Init(hSelfWindow))
        return FALSE;
    if (!ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext))
        return FALSE;

    return TRUE;
}

void Overlay::Render(std::function<void()> CallBack, bool& isMenuShow)
{
    while (bRunning)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                bRunning = false;
        }

        if (Type == WindowType::AttachWindow)
        {
            // 移动窗口保持在目标窗口前和窗口大小
            if (hTargetWindow != nullptr)
                MoveWindow();
        }
        else if (Type == WindowType::NewWindow)
        {
            // 如果不是前台窗口，切换前台
            if (!GetWindowStatus())
            {
                SetForegroundWindow(hSelfWindow);
            }
        }

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

        
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            CallBack();
        }
        ImGui::EndFrame();

        // 当菜单显示的时候 取消透明窗口
        if (isMenuShow)
            SetWindowLong(hSelfWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        else
            SetWindowLong(hSelfWindow, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

        ImGui::Render();
        float clear_color_with_alpha[4] = { CleanColor.x, CleanColor.y, CleanColor.z, CleanColor.w };
        pd3dDeviceContext->OMSetRenderTargets(1, &pMainRenderTargetView, nullptr);
        pd3dDeviceContext->ClearRenderTargetView(pMainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // pSwapChain->Present(1, 0); 是开启垂直同步
        pSwapChain->Present(0, 0);
    }

}


void Overlay::Destroy()
{
    bInit = false;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hSelfWindow);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

