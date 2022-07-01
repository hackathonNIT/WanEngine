#pragma once

#include<d3d12.h>

class GraphicsManager {
private:
public:
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;
	GraphicsManager(ID3D12Device* device);
	~GraphicsManager();
};