#pragma once



#include<d3d12.h>
#include<dxgi1_6.h>

class DeviceManager {
private:


public:
	ID3D12Device* device = nullptr;
	IDXGIFactory6* dxgiFactory = nullptr;
	
	DeviceManager();
	~DeviceManager();

};