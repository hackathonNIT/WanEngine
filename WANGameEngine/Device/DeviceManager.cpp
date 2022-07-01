#include "DeviceManager.h"
#include<d3d12.h>
#include<tchar.h>
#include <vector>
#include<iostream>

#pragma comment(lib,"d3d12.lib")

DeviceManager::DeviceManager()
{
    D3D_FEATURE_LEVEL levels[] = {
       D3D_FEATURE_LEVEL_12_1,
       D3D_FEATURE_LEVEL_12_0,
       D3D_FEATURE_LEVEL_11_1,
       D3D_FEATURE_LEVEL_11_0,
    };

    //�C�ӂ̃A�_�v�^�[��I������ꍇ
    //�A�_�v�^�[��
    CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    std::vector<IDXGIAdapter*> adapters;
    IDXGIAdapter* adapter = nullptr;//�g���A�_�v�^�[������
    IDXGIAdapter* tmpAdapter = nullptr;
    for (int i = 0; dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
        adapters.push_back(tmpAdapter);
    }
    for (auto a : adapters) {
        DXGI_ADAPTER_DESC adesc = {};
        a->GetDesc(&adesc);
        std::wstring strDesc = adesc.Description;

        if (strDesc.find(L"NVIDIA") != std::string::npos) {
            adapter = a;
            break;
        }
    }


    //Direct3D�f�o�C�X�̏�����
    D3D_FEATURE_LEVEL featureLevel;
    for (auto l : levels) {
        if (D3D12CreateDevice(adapter, l, IID_PPV_ARGS(&device)) == S_OK) {
            featureLevel = l;
            break;
        }
    }
}

DeviceManager::~DeviceManager()
{
}
