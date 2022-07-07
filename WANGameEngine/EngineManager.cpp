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
    //Result &= initializeResourceManager();
    Result &= initializeShaderManager();
    return Result;
}

bool EngineManager::MainLoopProcess()
{
    graphicsManager->showWindow();
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

        graphicsManager->draw();

        
        
        graphicsManager->resetCmd();
    }

    
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

    graphicsManager=new GraphicsManager(window_width, window_height);
    
    bool Result=graphicsManager->initializeGraphicsManager();

    

    
    
    return Result;
}

bool EngineManager::initializeResourceManager()
{
    
    

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
