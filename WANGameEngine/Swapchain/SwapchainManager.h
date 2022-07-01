#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>

class SwapchainManager {
private:

public:
	IDXGISwapChain4* swapchain = nullptr;

	SwapchainManager(IDXGIFactory6* dxgiFactory, const unsigned int window_width, const unsigned int window_height, ID3D12CommandQueue* cmdQueue, HWND hwnd);
	~SwapchainManager();
};