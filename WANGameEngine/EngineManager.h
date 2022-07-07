#pragma once
#include "Singleton.h"

#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<vector>

#include "Graphics/GraphicsManager.h"
#include "Shader/ShaderManager.h"




class EngineManager : public Singleton<EngineManager>
{
private:
	friend Singleton<EngineManager>;

	
	//描画
	GraphicsManager* graphicsManager = nullptr;
	//シェーダー
	ShaderManager* shaderManager = nullptr;

	//ウィンドウサイズ
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	
	EngineManager();
	~EngineManager();
public:
	
	bool initializeManager(); 
	bool MainLoopProcess();

	bool initializeGraphicsManager();//graphicの初期化
	bool initializeShaderManager();//Shaderの初期化

	void EnableDebugLayer();
};
