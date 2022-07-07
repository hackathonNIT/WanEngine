#include "GraphicsManager.h"
#include<tchar.h>
#include<d3d12.h>
#include<iostream>

#include<d3dcompiler.h>
#pragma comment(lib,"d3d12.lib")

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


GraphicsManager::GraphicsManager(const unsigned int window_width, const unsigned int window_height)
{
    this->window_height = window_height;
    this->window_width = window_width;

    viewport.Width = window_width;//�o�͐�̕�(�s�N�Z����)
    viewport.Height = window_height;//�o�͐�̍���(�s�N�Z����)
    viewport.TopLeftX = 0;//�o�͐�̍�����WX
    viewport.TopLeftY = 0;//�o�͐�̍�����WY
    viewport.MaxDepth = 1.0f;//�[�x�ő�l
    viewport.MinDepth = 0.0f;//�[�x�ŏ��l


    scissorrect.top = 0;//�؂蔲������W
    scissorrect.left = 0;//�؂蔲�������W
    scissorrect.right = scissorrect.left + window_width;//�؂蔲���E���W
    scissorrect.bottom = scissorrect.top + window_height;//�؂蔲�������W
}

GraphicsManager::~GraphicsManager()
{
    UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

bool GraphicsManager::initializeGraphicsManager()
{
    bool Result = initializeWindow();
    Result &= initializeDevice();
    Result &= initializeCommand();
    Result &= initializeSwapChain();
    Result &= initializeHeap();
    Result &= initializeBuffer();
    Result &= initializeFance();
    Result &= initializeResource();
    Result &= initializeView();
    Result &= initializeShader();

    return Result;
}

void GraphicsManager::showWindow()
{
    ShowWindow(hwnd, SW_SHOW);
}

void GraphicsManager::draw()
{
    //directx�̏���
    auto bbIdx = swapchain->GetCurrentBackBufferIndex();


    //TODO : GraphicsManager���ŏ����ł���悤�ɕύX����

    D3D12_RESOURCE_BARRIER BarrierDesc = {};
    BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    BarrierDesc.Transition.pResource = backBuffers[bbIdx];
    BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    cmdList->ResourceBarrier(1, &BarrierDesc);
    cmdList->SetPipelineState(pipelinestate);

    auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
    rtvH.ptr += bbIdx * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

    //��ʃN���A
    float clearColor[] = { 0.0f,1.0f,0.0f,1.0f };
    cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);


    cmdList->RSSetViewports(1, &viewport);
    cmdList->RSSetScissorRects(1, &scissorrect);
    cmdList->SetGraphicsRootSignature(rootsignature);

    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->IASetVertexBuffers(0, 1, &vbView);
    cmdList->IASetIndexBuffer(&ibView);


    //_cmdList->DrawInstanced(4, 1, 0, 0);
    cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);


    BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    cmdList->ResourceBarrier(1, &BarrierDesc);

    cmdList->Close();

    //�R�}���h���X�g�̎��s
    ID3D12CommandList* cmdlists[] = { cmdList };
    cmdQueue->ExecuteCommandLists(1, cmdlists);
    ////�҂�
    cmdQueue->Signal(fence, ++fenceVal);

    if (fence->GetCompletedValue() != fenceVal) {
        auto event = CreateEvent(nullptr, false, false, nullptr);
        fence->SetEventOnCompletion(fenceVal, event);
        WaitForSingleObject(event, INFINITE);
        CloseHandle(event);
    }
}

void GraphicsManager::resetCmd()
{
    cmdAllocator->Reset();//�L���[���N���A
    cmdList->Reset(cmdAllocator, nullptr);//�ĂуR�}���h���X�g�����߂鏀��

    //�t���b�v
    swapchain->Present(1, 0);
}

bool GraphicsManager::initializeWindow()
{
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;
    wndClass.lpszClassName = _T("DirectXTest");
    wndClass.hInstance = GetModuleHandle(0);
    RegisterClassEx(&wndClass);
    RECT wrc = { 0,0, window_width, window_height };
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
    //�E�B���h�E�I�u�W�F�N�g�̐���
    hwnd = CreateWindow(wndClass.lpszClassName,
        _T("�E�B���h�E"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        wrc.right - wrc.left,
        wrc.bottom - wrc.top,
        nullptr,
        nullptr,
        wndClass.hInstance,
        nullptr);
    return true;
}

bool GraphicsManager::initializeDevice()
{
    D3D_FEATURE_LEVEL levels[] = {
       D3D_FEATURE_LEVEL_12_1,
       D3D_FEATURE_LEVEL_12_0,
       D3D_FEATURE_LEVEL_11_1,
       D3D_FEATURE_LEVEL_11_0,
    };

    //�C�ӂ̃A�_�v�^�[��I������ꍇ
    //�A�_�v�^�[��
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    std::vector<IDXGIAdapter*> adapters;
    IDXGIAdapter* adapter = nullptr;//�g���A�_�v�^�[������
    IDXGIAdapter* tmpAdapter = nullptr;
    for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
        adapters.push_back(tmpAdapter);
    }
    for (auto a : adapters) {
        DXGI_ADAPTER_DESC adesc = {};
        a->GetDesc(&adesc);
        std::wstring strDesc = adesc.Description;

        if (strDesc.find(L"NVIDIA") != std::string::npos) {
            adapter = a;
            break;
        }
    }


    //Direct3D�f�o�C�X�̏�����
    D3D_FEATURE_LEVEL featureLevel;
    for (auto l : levels) {
        if (D3D12CreateDevice(adapter, l, IID_PPV_ARGS(&device)) == S_OK) {
            featureLevel = l;
            break;
        }
    }
    return true;
}

bool GraphicsManager::initializeCommand()
{
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList));
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.NodeMask = 0;
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
    return true;
}

bool GraphicsManager::initializeSwapChain()
{
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
    return true;
}

bool GraphicsManager::initializeHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 2;//�\���̂Q��
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
    return true;
}

bool GraphicsManager::initializeBuffer()
{
    DXGI_SWAP_CHAIN_DESC swcDesc = {};
    swapchain->GetDesc(&swcDesc);
    std::cout << swcDesc.BufferCount << std::endl;
    backBuffers = std::vector<ID3D12Resource*>(swcDesc.BufferCount);
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
    for (size_t i = 0; i < swcDesc.BufferCount; ++i) {
        swapchain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&backBuffers[i]));
        device->CreateRenderTargetView(backBuffers[i], nullptr, handle);
        handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    return true;
}

bool GraphicsManager::initializeFance()
{
    device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    return true;
}

bool GraphicsManager::initializeResource()
{
    heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resdesc.Width = sizeof(vertices);
    resdesc.Height = 1;
    resdesc.DepthOrArraySize = 1;
    resdesc.MipLevels = 1;
    resdesc.Format = DXGI_FORMAT_UNKNOWN;
    resdesc.SampleDesc.Count = 1;
    resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    device->CreateCommittedResource(
        &heapprop,
        D3D12_HEAP_FLAG_NONE,
        &resdesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertBuff));

    

    return true;
}

bool GraphicsManager::initializeView()
{
    DirectX::XMFLOAT3* vertMap = nullptr;
    vertBuff->Map(0, nullptr, (void**)&vertMap);
    std::copy(std::begin(vertices), std::end(vertices), vertMap);
    vertBuff->Unmap(0, nullptr);

    vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
    vbView.SizeInBytes = sizeof(vertices);
    vbView.StrideInBytes = sizeof(vertices[0]);


    unsigned short indices[] = { 0,1,2, 2,1,3 };

    ID3D12Resource* idxBuff = nullptr;

    resdesc.Width = sizeof(indices);
    device->CreateCommittedResource(
        &heapprop,
        D3D12_HEAP_FLAG_NONE,
        &resdesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&idxBuff)
    );

    unsigned short* mappedIdx = nullptr;
    idxBuff->Map(0, nullptr, (void**)&mappedIdx);
    std::copy(std::begin(indices), std::end(indices), mappedIdx);
    idxBuff->Unmap(0, nullptr);
    
    //�C���f�b�N�X�o�b�t�@�r���[���쐬
    D3D12_INDEX_BUFFER_VIEW ibView = {};
    ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
    ibView.Format = DXGI_FORMAT_R16_UINT;
    ibView.SizeInBytes = sizeof(indices);

    return true;
}

bool GraphicsManager::initializeShader()
{
    ID3DBlob* _vsBlob = nullptr;
    ID3DBlob* _psBlob = nullptr;

    ID3DBlob* errorBlob = nullptr;
    HRESULT result = D3DCompileFromFile(L"Shader/BasicVertexShader.hlsl",
        nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "BasicVS", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, &_vsBlob, &errorBlob);
    if (FAILED(result)) {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            std::cout << "BasicVertexShader�t�@�C������������܂���" << std::endl;
        }
        else {
            std::string errstr;
            errstr.resize(errorBlob->GetBufferSize());
            std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
            errstr += "\n";
            std::cout << errstr.c_str() << std::endl;
        }
        exit(1);
    }
    result = D3DCompileFromFile(L"Shader/BasicPixelShader.hlsl",
        nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "BasicPS", "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, &_psBlob, &errorBlob);
    if (FAILED(result)) {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            std::cout << "BasicPixelShader�t�@�C������������܂���" << std::endl;
        }
        else {
            std::string errstr;
            errstr.resize(errorBlob->GetBufferSize());
            std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
            errstr += "\n";
            std::cout << errstr.c_str() << std::endl;
        }
        exit(1);
    }
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
    gpipeline.pRootSignature = nullptr;
    gpipeline.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
    gpipeline.VS.BytecodeLength = _vsBlob->GetBufferSize();
    gpipeline.PS.pShaderBytecode = _psBlob->GetBufferPointer();
    gpipeline.PS.BytecodeLength = _psBlob->GetBufferSize();

    gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//���g��0xffffffff

    //
    gpipeline.BlendState.AlphaToCoverageEnable = false;
    gpipeline.BlendState.IndependentBlendEnable = false;

    D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};

    //�ЂƂ܂����Z���Z�⃿�u�����f�B���O�͎g�p���Ȃ�
    renderTargetBlendDesc.BlendEnable = false;
    renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    //�ЂƂ܂��_�����Z�͎g�p���Ȃ�
    renderTargetBlendDesc.LogicOpEnable = false;

    gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;


    gpipeline.RasterizerState.MultisampleEnable = false;//�܂��A���`�F���͎g��Ȃ�
    gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
    gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//���g��h��Ԃ�
    gpipeline.RasterizerState.DepthClipEnable = true;//�[�x�����̃N���b�s���O�͗L����

    //�c��
    gpipeline.RasterizerState.FrontCounterClockwise = false;
    gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    gpipeline.RasterizerState.AntialiasedLineEnable = false;
    gpipeline.RasterizerState.ForcedSampleCount = 0;
    gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


    gpipeline.DepthStencilState.DepthEnable = false;
    gpipeline.DepthStencilState.StencilEnable = false;

    gpipeline.InputLayout.pInputElementDescs = inputLayout;//���C�A�E�g�擪�A�h���X
    gpipeline.InputLayout.NumElements = _countof(inputLayout);//���C�A�E�g�z��

    gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//�X�g���b�v���̃J�b�g�Ȃ�
    gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//�O�p�`�ō\��

    gpipeline.NumRenderTargets = 1;//���͂P�̂�
    gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`1�ɐ��K�����ꂽRGBA

    gpipeline.SampleDesc.Count = 1;//�T���v�����O��1�s�N�Z���ɂ��P
    gpipeline.SampleDesc.Quality = 0;//�N�I���e�B�͍Œ�

    

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* rootSigBlob = nullptr;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
    device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
    rootSigBlob->Release();

    gpipeline.pRootSignature = rootsignature;
    
    device->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));

    

    return true;
}
