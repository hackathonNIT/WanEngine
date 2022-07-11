#pragma once
#include "../WindowBase.h"

class BlockList:
	public WindowBase{
private:
	unsigned int window_width;
	unsigned int window_height;

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	ID3D12PipelineState* pipelinestate = nullptr;

	ID3D12RootSignature* rootsignature = nullptr;

	D3D12_VIEWPORT viewport = {};
	D3D12_RECT scissorrect = {};

	double y;

public:
	using WindowBase::WindowBase;
	BlockList(const unsigned int window_width, const unsigned int window_height, ID3D12Device* device);
	~BlockList();

	bool getPipeline();

	bool process()override;
	bool draw(ID3D12GraphicsCommandList*& cmdList)override;


};