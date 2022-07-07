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

	
	//�`��
	GraphicsManager* graphicsManager = nullptr;
	//�V�F�[�_�[
	ShaderManager* shaderManager = nullptr;

	//�E�B���h�E�T�C�Y
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	
	EngineManager();
	~EngineManager();
public:
	
	bool initializeManager(); 
	bool MainLoopProcess();

	bool initializeGraphicsManager();//graphic�̏�����
	bool initializeShaderManager();//Shader�̏�����

	void EnableDebugLayer();
};
