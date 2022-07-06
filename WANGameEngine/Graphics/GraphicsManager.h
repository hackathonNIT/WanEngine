#pragma once


#include<d3d12.h>
#include<dxgi1_6.h>

class GraphicsManager {
private:
public:
	HWND hwnd;
	WNDCLASSEX wndClass = {};
	ID3D12Device* device;
	IDXGIFactory6* dxgiFactory = nullptr;
	unsigned int window_width;
	unsigned int window_height;
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;
	GraphicsManager(const unsigned int window_width, const unsigned int window_height);
	~GraphicsManager();

	bool initializeGraphicsManager();
	bool initializeWindow();
	bool initializeDevice();
	bool initializeCommand();
};