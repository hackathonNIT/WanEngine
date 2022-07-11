#pragma once
#include "../WindowBase.h"

class BlockList:
	public WindowBase{
private:

public:
	BlockList(const unsigned int window_width, const unsigned int window_height);
	~BlockList();

	bool Process();
	bool Draw(ID3D12GraphicsCommandList*& cmdList);


};