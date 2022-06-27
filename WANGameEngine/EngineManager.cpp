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
    Result &= initializeDeviseManager();
    Result &= initializeGraphicsManager();
    return Result;
}

bool EngineManager::MainLoopProcess()
{
    ShowWindow(_hwnd, SW_SHOW);
    MSG msg = {};
    //MainLoop
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

bool EngineManager::initializeDeviseManager()
{
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    //任意のアダプターを選択する場合
    //アダプター列挙
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    std::vector<IDXGIAdapter*> adapters;
    IDXGIAdapter* adapter = nullptr;//使うアダプターを入れる
    IDXGIAdapter* tmpAdapter = nullptr;
    for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
        adapters.push_back(tmpAdapter);
    }
    for (auto a : adapters) {
        DXGI_ADAPTER_DESC adesc = {};
        a->GetDesc(&adesc);
        std::wstring strDesc = adesc.Description;

        if (strDesc.find(L"NVIDIA")!=std::string::npos) {
            adapter = a;
            break;
        }
    }


    //Direct3Dデバイスの初期化
    D3D_FEATURE_LEVEL featureLevel;
    for (auto l : levels) {
        if (D3D12CreateDevice(adapter, l, IID_PPV_ARGS(&device)) == S_OK) {
            featureLevel = l;
            break;
        }
    }

    return true;
}

bool EngineManager::initializeGraphicsManager()
{


    return true;
}

void EngineManager::EnableDebugLayer()
{
    ID3D12Debug* debugLayer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    debugLayer->EnableDebugLayer();
    debugLayer->Release();
}
