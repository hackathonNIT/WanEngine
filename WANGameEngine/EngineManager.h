#pragma once
#include "Singleton.h"

#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<vector>

#include "Window/WindowManager.h"
#include "Device/DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include "Swapchain/SwapchainManager.h"
#include "Resource/ResourceManager.h"
#include "Shader/ShaderManager.h"




class EngineManager : public Singleton<EngineManager>
{
private:
	friend Singleton<EngineManager>;

	//ウィンドウ
	WindowManager* windowMamager = nullptr;

	//デバイス
	DeviceManager* deviceManager = nullptr;

	//スワップチェーン
	SwapchainManager* swapchainManager = nullptr;

	
	//コマンド
	GraphicsManager* graphicsManager = nullptr;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	
	

	//リソース
	ResourceManager* resourceManager = nullptr;

	//シェーダー
	ShaderManager* shaderManager = nullptr;

	//ウィンドウサイズ
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	
	EngineManager();
	~EngineManager();
public:
	
	bool initializeManager(); 
	bool MainLoopProcess();
	
	bool initializeWindowManager();//windowの初期化
	bool initializeDeviceManager();//deviceの初期化
	bool initializeGraphicsManager();//graphicの初期化
	bool initializeResourceManager();//resourceの初期化
	bool initializeShaderManager();//Shaderの初期化

	void EnableDebugLayer();
};
