#pragma once


#include<d3d12.h>

class WindowManager {
private:
	

public:
	HWND hwnd;
	WNDCLASSEX wndClass = {};
	WindowManager(unsigned int window_width, unsigned int window_height);
	~WindowManager();

};