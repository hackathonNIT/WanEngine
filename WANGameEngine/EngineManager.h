#pragma once
#include "Singleton.h"

#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<vector>


class EngineManager : public Singleton<EngineManager>
{
private:
	friend Singleton<EngineManager>;

	//ウィンドウ
	HWND hwnd;
	WNDCLASSEX wndClass = {};

	//デバイス
	ID3D12Device* device = nullptr;
	IDXGIFactory6* dxgiFactory = nullptr;
	IDXGISwapChain4* swapchain = nullptr;
	
	//コマンド
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	
	std::vector<ID3D12Resource*> backBuffers;

	//フェンス
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	//ウィンドウサイズ
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;
	EngineManager();
	~EngineManager();
public:
	
	bool initializeManager(); 
	bool MainLoopProcess();
	
	bool initializeWindowManager();//windowの初期化
	bool initializeDeviseManager();//deviceの初期化
	bool initializeGraphicsManager();//graphicの初期化
	void EnableDebugLayer();
};
