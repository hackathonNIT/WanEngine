#pragma once


#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>

class GraphicsManager {
private:

	HWND hwnd;
	WNDCLASSEX wndClass = {};

	//�f�o�C�X
	ID3D12Device* device;
	IDXGIFactory6* dxgiFactory = nullptr;

	//���
	unsigned int window_width;
	unsigned int window_height;

	//�R�}���h
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;

	//�X���b�v�`�F�[��
	IDXGISwapChain4* swapchain = nullptr;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	std::vector<ID3D12Resource*> backBuffers;

	//�t�F���X
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	DirectX::XMFLOAT3 vertices[3] = {
		{-0.5f,-0.7f,0.0f} ,//����
		{-0.0f,0.7f,0.0f} ,//����
		{0.5f,-0.7f,0.0f} //�E��
	};
	D3D12_HEAP_PROPERTIES heapprop = {};
	D3D12_RESOURCE_DESC resdesc = {};
	ID3D12Resource* vertBuff = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};

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