#pragma once

#include <iostream>
#include <d3d12.h>

class ShaderManager {

private:
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    
public:
    ShaderManager();
    ~ShaderManager();

    bool readShader(LPCWSTR file, LPCSTR funcName, LPCSTR target, ID3DBlob* blob);
};
