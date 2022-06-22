#pragma once
#include "Singleton.h"

#include<Windows.h>


class EngineManager : public Singleton<EngineManager>
{
private:
	friend Singleton<EngineManager>;
	HWND _hwnd;
	WNDCLASSEX _wndClass = {};

	//ウィンドウサイズ
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;
	EngineManager();
	~EngineManager();
public:
	
	bool initializeManager();
	bool MainLoopProcess();
	
	bool initializeWindowManager();
	void EnableDebugLayer();
};
