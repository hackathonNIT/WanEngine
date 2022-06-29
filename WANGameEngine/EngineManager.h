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

	//�E�B���h�E
	HWND hwnd;
	WNDCLASSEX wndClass = {};

	//�f�o�C�X
	ID3D12Device* device = nullptr;
	IDXGIFactory6* dxgiFactory = nullptr;
	IDXGISwapChain4* swapchain = nullptr;
	
	//�R�}���h
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	
	std::vector<ID3D12Resource*> backBuffers;

	//�t�F���X
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

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
