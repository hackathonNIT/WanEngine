#pragma once
#include "Singleton.h"

#include<Windows.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<vector>

#include "Window/WindowManager.h"
#include "Device/DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include "Swapchain/SwapchainManager.h"
#include "Resource/ResourceManager.h"
#include "Shader/ShaderManager.h"




class EngineManager : public Singleton<EngineManager>
{
private:
	friend Singleton<EngineManager>;

	//�E�B���h�E
	WindowManager* windowMamager = nullptr;

	//�f�o�C�X
	DeviceManager* deviceManager = nullptr;

	//�X���b�v�`�F�[��
	SwapchainManager* swapchainManager = nullptr;

	
	//�R�}���h
	GraphicsManager* graphicsManager = nullptr;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	
	

	//���\�[�X
	ResourceManager* resourceManager = nullptr;

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
	
	bool initializeWindowManager();//window�̏�����
	bool initializeDeviceManager();//device�̏�����
	bool initializeGraphicsManager();//graphic�̏�����
	bool initializeResourceManager();//resource�̏�����
	bool initializeShaderManager();//Shader�̏�����

	void EnableDebugLayer();
};
