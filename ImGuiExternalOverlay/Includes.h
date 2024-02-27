#ifndef INCLUDES_H
#define INCLUDES_H

// 系统库
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <functional>
#include <tchar.h>

#include <dwmapi.h>
#pragma comment(lib, "dwmapi")
#include <d3d11.h>
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxgi")


// 界面库
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "Overlay.h"

#endif
