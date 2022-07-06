#include "EngineManager.h"

#include<tchar.h>
#include<d3d12.h>

#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>

#include<d3dcompiler.h>

#include<iostream>


#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3dcompiler.lib")



EngineManager::EngineManager()
{
}

EngineManager::~EngineManager()
{
}

bool EngineManager::initializeManager()
{
    bool Result = false;
    //Result = initializeWindowManager();
    //Result &= initializeDeviceManager();
    Result = initializeGraphicsManager();
    Result &= initializeResourceManager();
    Result &= initializeShaderManager();
    return Result;
}

bool EngineManager::MainLoopProcess()
{
    ShowWindow(graphicsManager->hwnd, SW_SHOW);
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

        //directxの処理
        auto bbIdx = swapchainManager->swapchain->GetCurrentBackBufferIndex();
        

        //TODO : GraphicsManager内で処理できるように変更する

        D3D12_RESOURCE_BARRIER BarrierDesc = {};
        BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        BarrierDesc.Transition.pResource = backBuffers[bbIdx];
        BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        graphicsManager->cmdList->ResourceBarrier(1, &BarrierDesc);

        auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
        rtvH.ptr += bbIdx * graphicsManager->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        graphicsManager->cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

        //画面クリア
        float clearColor[] = { 1.0f,1.0f,0.0f,1.0f };
        graphicsManager->cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

        BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        graphicsManager->cmdList->ResourceBarrier(1, &BarrierDesc);

        graphicsManager->cmdList->Close();

        //コマンドリストの実行
        ID3D12CommandList* cmdlists[] = { graphicsManager->cmdList };
        graphicsManager->cmdQueue->ExecuteCommandLists(1, cmdlists);
        ////待ち
        graphicsManager->cmdQueue->Signal(fence, ++fenceVal);

        if (fence->GetCompletedValue() != fenceVal) {
            auto event = CreateEvent(nullptr, false, false, nullptr);
            fence->SetEventOnCompletion(fenceVal, event);
            WaitForSingleObject(event, INFINITE);
            CloseHandle(event);
        }
        graphicsManager->cmdAllocator->Reset();//キューをクリア
        graphicsManager->cmdList->Reset(graphicsManager->cmdAllocator, nullptr);//再びコマンドリストをためる準備

        //フリップ
        swapchainManager->swapchain->Present(1, 0);

    }

    UnregisterClass(graphicsManager->wndClass.lpszClassName, graphicsManager->wndClass.hInstance);
    return true;
}





/**************************initialize**************************/


bool EngineManager::initializeWindowManager()
{
    windowMamager = new WindowManager(window_width, window_height);
    if (windowMamager->hwnd == 0)return false;
#ifdef _DEBUG
    //デバッグレイヤーをオンに
    EnableDebugLayer();
#endif
    return true;
}

bool EngineManager::initializeDeviceManager()
{
    deviceManager = new DeviceManager();

    return true;
}

bool EngineManager::initializeGraphicsManager()
{
    //コマンドリスト
    graphicsManager=new GraphicsManager(window_width, window_height);
    
    bool Result=graphicsManager->initializeGraphicsManager();

    //スワップチェーン
    swapchainManager = new SwapchainManager(
        graphicsManager->dxgiFactory,
        window_width, window_height,
        graphicsManager->cmdQueue,
        graphicsManager->hwnd
    );

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 2;//表裏の２つ
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    graphicsManager->device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
    DXGI_SWAP_CHAIN_DESC swcDesc = {};
    swapchainManager->swapchain->GetDesc(&swcDesc);
    std::cout << swcDesc.BufferCount << std::endl;
    backBuffers = std::vector<ID3D12Resource*>(swcDesc.BufferCount);
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
    for (size_t i = 0; i < swcDesc.BufferCount; ++i) {
        swapchainManager->swapchain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&backBuffers[i]));
        graphicsManager->device->CreateRenderTargetView(backBuffers[i], nullptr, handle);
        handle.ptr += graphicsManager->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    
    graphicsManager->device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

    return Result;
}

bool EngineManager::initializeResourceManager()
{
    
    resourceManager = new ResourceManager();
    
    graphicsManager->device->CreateCommittedResource(
        &resourceManager->heapprop,
        D3D12_HEAP_FLAG_NONE,
        &resourceManager->resdesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resourceManager->vertBuff));


    //書き込み
    resourceManager->write();

    return true;
}

bool EngineManager::initializeShaderManager()
{
    shaderManager = new ShaderManager();
    return true;
}

void EngineManager::EnableDebugLayer()
{
    ID3D12Debug* debugLayer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    debugLayer->EnableDebugLayer();
    debugLayer->Release();
}
