#include "EngineManager.h"

#include<tchar.h>
#include<d3d12.h>

#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>

#include<d3dcompiler.h>

#include<d3dx12.h>

#include<iostream>


#include<d3dcompiler.h>

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
    Result = initializeWindowManager();
    Result &= initializeDeviceManager();
    Result &= initializeGraphicsManager();
    Result &= initializeResourceManager();
    Result &= initializeShaderManager();
    return Result;
}

bool EngineManager::MainLoopProcess()
{
    ShowWindow(windowMamager->hwnd, SW_SHOW);
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
        
        D3D12_RESOURCE_BARRIER BarrierDesc = {};
        BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        BarrierDesc.Transition.pResource = backBuffers[bbIdx];
        BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        cmdList->ResourceBarrier(1, &BarrierDesc);

        auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
        rtvH.ptr += bbIdx * deviceManager->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

        //画面クリア
        float clearColor[] = { 1.0f,1.0f,0.0f,1.0f };
        cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

        BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        cmdList->ResourceBarrier(1, &BarrierDesc);

        cmdList->Close();

        //コマンドリストの実行
        ID3D12CommandList* cmdlists[] = { cmdList };
        cmdQueue->ExecuteCommandLists(1, cmdlists);
        ////待ち
        cmdQueue->Signal(fence, ++fenceVal);

        if (fence->GetCompletedValue() != fenceVal) {
            auto event = CreateEvent(nullptr, false, false, nullptr);
            fence->SetEventOnCompletion(fenceVal, event);
            WaitForSingleObject(event, INFINITE);
            CloseHandle(event);
        }
        cmdAllocator->Reset();//キューをクリア
        cmdList->Reset(cmdAllocator, nullptr);//再びコマンドリストをためる準備

        //フリップ
        swapchainManager->swapchain->Present(1, 0);

    }

    UnregisterClass(windowMamager->wndClass.lpszClassName, windowMamager->wndClass.hInstance);
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
    deviceManager->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
    deviceManager->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList));
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.NodeMask = 0;
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    deviceManager->device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

    //スワップチェーン
    swapchainManager = new SwapchainManager(
        deviceManager->dxgiFactory,
        window_width, window_height,
        cmdQueue,
        windowMamager->hwnd
    );

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 2;//表裏の２つ
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    deviceManager->device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
    DXGI_SWAP_CHAIN_DESC swcDesc = {};
    swapchainManager->swapchain->GetDesc(&swcDesc);
    std::cout << swcDesc.BufferCount << std::endl;
    backBuffers = std::vector<ID3D12Resource*>(swcDesc.BufferCount);
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
    for (size_t i = 0; i < swcDesc.BufferCount; ++i) {
        swapchainManager->swapchain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&backBuffers[i]));
        deviceManager->device->CreateRenderTargetView(backBuffers[i], nullptr, handle);
        handle.ptr += deviceManager->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    
    deviceManager->device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

    return true;
}

bool EngineManager::initializeResourceManager()
{
    DirectX::XMFLOAT3 vertices[] = {
        {-0.4f,-0.7f,0.0f} ,//左下
        {-0.4f,0.7f,0.0f} ,//左上
        {0.4f,-0.7f,0.0f} ,//右下
        {0.4f,0.7f,0.0f} ,//右上
    };

    D3D12_HEAP_PROPERTIES heapprop = {};
    heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;


    D3D12_RESOURCE_DESC resdesc = {};
    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resdesc.Width = sizeof(vertices);
    resdesc.Height = 1;
    resdesc.DepthOrArraySize = 1;
    resdesc.MipLevels = 1;
    resdesc.Format = DXGI_FORMAT_UNKNOWN;
    resdesc.SampleDesc.Count = 1;
    resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    //UPLOAD(確保は可能)
    ID3D12Resource* vertBuff = nullptr;
    deviceManager->device->CreateCommittedResource(
        &heapprop,
        D3D12_HEAP_FLAG_NONE,
        &resdesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff));

    
    //バッファーに頂点情報をコピー
    DirectX::XMFLOAT3* vertMap = nullptr;
    vertBuff->Map(0, nullptr, (void**)&vertMap);
    std::copy(std::begin(vertices), std::end(vertices), vertMap);
    vertBuff->Unmap(0, nullptr);
    D3D12_VERTEX_BUFFER_VIEW vbView = {};
    vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeof(vertices);
    vbView.StrideInBytes = sizeof(vertices[0]);

    return true;
}

bool EngineManager::initializeShaderManager()
{
    ID3DBlob* _vsBlob = nullptr;
    ID3DBlob* _psBlob = nullptr;

    ID3DBlob* errorBlob = nullptr;

    //後で関数とかにして使いやすくする
    auto result = D3DCompileFromFile(L"Shader/BasicVertexShader.hlsl",
        nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "BasicVS", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, &_vsBlob, &errorBlob);
    if (FAILED(result)) {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            std::cout << "ファイルが見当たりません"<<std::endl;
        }
        else {
            std::string errstr;
            errstr.resize(errorBlob->GetBufferSize());
            std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
            errstr += "\n";
            std::cout << errstr.c_str() << std::endl;
        }
        return false;
    }
    result = D3DCompileFromFile(L"Shader/BasicPixelShader.hlsl",
        nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "BasicPS", "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, &_psBlob, &errorBlob);
    if (FAILED(result)) {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            std::cout << "ファイルが見当たりません" << std::endl;;
        }
        else {
            std::string errstr;
            errstr.resize(errorBlob->GetBufferSize());
            std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
            errstr += "\n";
            std::cout << errstr.c_str() << std::endl;
        }
        return false;
    }


    return true;
}

void EngineManager::EnableDebugLayer()
{
    ID3D12Debug* debugLayer = nullptr;
    auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    debugLayer->EnableDebugLayer();
    debugLayer->Release();
}
