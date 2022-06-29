#pragma once
#include "Singleton.h"

#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include "Dx12Wrapper.h"


class EngineManager : public Singleton<EngineManager>
{
private:
	friend Singleton<EngineManager>;
	HWND hwnd;
	WNDCLASSEX wndClass = {};

	ID3D12Device* device = nullptr;
	IDXGIFactory6* dxgiFactory = nullptr;
	IDXGISwapChain4* swapchain = nullptr;
	
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;
	//�E�B���h�E�T�C�Y
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;
	EngineManager();
	~EngineManager();
public:
	
	bool initializeManager(); 
	bool MainLoopProcess();
	
	bool initializeWindowManager();//window�̏�����
	bool initializeDeviseManager();//device�̏�����
	bool initializeGraphicsManager();//graphic�̏�����
	void EnableDebugLayer();
};
