#include "ShaderManager.h"

#include<d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

ShaderManager::ShaderManager()
{
    readShader(L"Shader/BasicVertexShader.hlsl", "BasicVS", "vs_5_0", vsBlob);
    readShader(L"Shader/BasicPixelShader.hlsl", "BasicPS","ps_5_0", psBlob);
}

ShaderManager::~ShaderManager()
{
}

bool ShaderManager::readShader(LPCWSTR file, LPCSTR funcName, LPCSTR target, ID3DBlob* blob)
{
    ID3DBlob* errorBlob = nullptr;
    auto result = D3DCompileFromFile(file,
        nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        funcName, target,
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, &blob, &errorBlob);
    if (FAILED(result)) {
        if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            std::cout << file << "ƒtƒ@ƒCƒ‹‚ªŒ©“–‚½‚è‚Ü‚¹‚ñ" << std::endl;
        }
        else {
            std::string errstr;
            errstr.resize(errorBlob->GetBufferSize());
            std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
            errstr += "\n";
            std::cout << errstr.c_str() << std::endl;
        }
        return false;
    }
	return true;
}
