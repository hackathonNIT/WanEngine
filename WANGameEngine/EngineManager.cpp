#include "EngineManager.h"

#include<tchar.h>
#include<d3d12.h>

#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>

#include<d3dcompiler.h>

#include<d3dx12.h>

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


EngineManager::EngineManager()
{
}

EngineManager::~EngineManager()
{
}

bool EngineManager::initializeManager()
{
    bool Result=false;
    Result=initializeWindowManager();
    return Result;
}

bool EngineManager::MainLoopProcess()
{
    ShowWindow(_hwnd, SW_SHOW);
    MSG msg = {};
    while (1) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) {
            break;
        }

    }

    UnregisterClass(_wndClass.lpszClassName, _wndClass.hInstance);
    return true;
}

bool EngineManager::initializeWindowManager()
{
    _wndClass.cbSize = sizeof(WNDCLASSEX);
    _wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;
    _wndClass.lpszClassName = _T("DirectXTest");
    _wndClass.hInstance = GetModuleHandle(0);
    RegisterClassEx(&_wndClass);
    RECT wrc = { 0,0, window_width, window_height };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
    //ウィンドウオブジェクトの生成
    _hwnd = CreateWindow(_wndClass.lpszClassName,
        _T("わんえんじん"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        _wndClass.hInstance,
        nullptr);
    if (_hwnd == 0)return false;
#ifdef _DEBUG
    //デバッグレイヤーをオンに
    EnableDebugLayer();
#endif
    return true;
}

void EngineManager::EnableDebugLayer()
{
    ID3D12Debug* debugLayer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    debugLayer->EnableDebugLayer();
    debugLayer->Release();
}
