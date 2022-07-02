#pragma once

#include<d3dcompiler.h>

#include<d3dx12.h>


#include<dxgi1_6.h>
#include<DirectXMath.h>

class ResourceManager {
private:
    
public:
	DirectX::XMFLOAT3 vertices[4] = {
		{-0.4f,-0.7f,0.0f} ,//ç∂â∫
		{-0.4f,0.7f,0.0f} ,//ç∂è„
		{0.4f,-0.7f,0.0f} ,//âEâ∫
		{0.4f,0.7f,0.0f} ,//âEè„
	};
	D3D12_HEAP_PROPERTIES heapprop = {};
	D3D12_RESOURCE_DESC resdesc = {};
	ID3D12Resource* vertBuff = nullptr;
	ResourceManager();
	~ResourceManager();

	bool write();
};