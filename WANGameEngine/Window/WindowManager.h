#pragma once


#include<d3d12.h>

class WindowManager {
private:
	

public:
	HWND hwnd;
	WNDCLASSEX wndClass = {};
	WindowManager(const unsigned int window_width, const unsigned int window_height);
	~WindowManager();

};