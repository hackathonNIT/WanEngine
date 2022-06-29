#include "EngineManager.h"

#include<tchar.h>
#include<d3d12.h>

#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>

#include<d3dcompiler.h>

#include<d3dx12.h>

#include<iostream>

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
    ShowWindow(hwnd, SW_SHOW);
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

    UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
    return true;
}

bool EngineManager::initializeWindowManager()
{
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;
    wndClass.lpszClassName = _T("DirectXTest");
    wndClass.hInstance = GetModuleHandle(0);
    RegisterClassEx(&wndClass);
    RECT wrc = { 0,0, window_width, window_height };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
    //ウィンドウオブジェクトの生成
    hwnd = CreateWindow(wndClass.lpszClassName,
        _T("わんえんじん"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        wndClass.hInstance,
        nullptr);
    if (hwnd == 0)return false;
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
    //コマンドリスト
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList));
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.NodeMask = 0;
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

    //スワップチェーン
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
    swapchainDesc.Width = window_width;
    swapchainDesc.Height = window_height;
    swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.Stereo = false;
    swapchainDesc.SampleDesc.Count = 1;
    swapchainDesc.SampleDesc.Quality = 0;
    swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
    swapchainDesc.BufferCount = 2;
    swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    dxgiFactory->CreateSwapChainForHwnd(cmdQueue, hwnd, &swapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)&swapchain);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 2;//表裏の２つ
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12DescriptorHeap* rtvHeaps = nullptr;
    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
    DXGI_SWAP_CHAIN_DESC swcDesc = {};
    swapchain->GetDesc(&swcDesc);
    std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
    for (size_t i = 0; i < swcDesc.BufferCount; ++i) {
        swapchain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&_backBuffers[i]));
        device->CreateRenderTargetView(_backBuffers[i], nullptr, handle);
        handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    ID3D12Fence* fence = nullptr;
    UINT64 fenceVal = 0;
    device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));



    return true;
}

void EngineManager::EnableDebugLayer()
{
    ID3D12Debug* debugLayer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    debugLayer->EnableDebugLayer();
    debugLayer->Release();
}
