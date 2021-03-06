#pragma once


#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>

#include "../Block/BlockList.h"

class GraphicsManager {
private:

	HWND hwnd;
	WNDCLASSEX wndClass = {};

	//デバイス
	ID3D12Device* device;
	IDXGIFactory6* dxgiFactory = nullptr;

	//画面
	unsigned int window_width;
	unsigned int window_height;

	BlockList* blocklist;
	//コマンド
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;

	//スワップチェーン
	IDXGISwapChain4* swapchain = nullptr;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	std::vector<ID3D12Resource*> backBuffers;

	//フェンス
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	
	D3D12_HEAP_PROPERTIES heapprop = {};
	D3D12_RESOURCE_DESC resdesc = {};
	ID3D12Resource* vertBuff = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	D3D12_VERTEX_BUFFER_VIEW vbView2 = {};
	D3D12_INDEX_BUFFER_VIEW ibView2 = {};

	ID3D12PipelineState* pipelinestate = nullptr;

	ID3D12RootSignature* rootsignature = nullptr;

	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorrect = {};

	bool initializeWindow();
	bool initializeDevice();
	bool initializeCommand();
	bool initializeSwapChain();
	bool initializeHeap();
	bool initializeBuffer();
	bool initializeFance();
	bool initializeResource();
	bool initializeView();
	bool initializeShader();

public:
	

	GraphicsManager(const unsigned int window_width, const unsigned int window_height);
	~GraphicsManager();

	bool initializeGraphicsManager();

	//void showWindow();

	void draw();

	void resetCmd();
	
};