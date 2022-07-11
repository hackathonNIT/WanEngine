#pragma once

#include<d3d12.h>

class WindowBase {
private:
	
public:
	WindowBase() {};
	virtual ~WindowBase() {};
	virtual bool process() = 0;
	virtual bool draw(ID3D12GraphicsCommandList* &cmdList) = 0;
};