#pragma once

#include "rhi_d3d12_header.h"
#include "../rhi_shader.h"
#include <vector>

class D3D12Device;

class D3D12Shader : public RHIShader
{
public:
    D3D12Shader(D3D12Device* pDevice, const RHIShaderDesc& desc, const std::vector<uint8_t> data, const std::string& name);

    virtual void* GetHandle() const override { return nullptr; }

    D3D12_SHADER_BYTECODE GetByteCode() const { return { m_data.data(), m_data.size()}; }

private:
    std::vector<uint8_t> m_data;
};