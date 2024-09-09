#pragma once

#include "rhi_d3d12_header.h"
#include "../rhi_descriptor.h"
#include "../rhi_buffer.h"

class D3D12Device;

class D3D12ShaderResourceView : public RHIDescriptor
{
public:
    D3D12ShaderResourceView(D3D12Device* pDevice, RHIResource* pResource, const RHIShaderResourceViewDesc& desc, const std::string& name);
    ~D3D12ShaderResourceView();

    virtual void* GetHandle() const override { return m_pResource->GetHandle(); }
    virtual uint32_t GetIndex() const override { return m_descriptor.index; }

    bool Create();
private:
    RHIResource* m_pResource = nullptr;
    RHIShaderResourceViewDesc m_desc = {};
    D3D12Descriptor m_descriptor;
};

class D3D12UnorderedAccessView : public RHIDescriptor
{
public:
    D3D12UnorderedAccessView(D3D12Device* pDevice, RHIResource* pResource, const RHIUnorderedAccessViewDesc& desc, const std::string& name);
    ~D3D12UnorderedAccessView();

    virtual void* GetHandle() const override { return m_pResource->GetHandle(); }
    virtual uint32_t GetIndex() const override { return m_descriptor.index; }

    bool Create();
private:
    RHIResource* m_pResource = nullptr;
    RHIUnorderedAccessViewDesc m_desc = {};
    D3D12Descriptor m_descriptor;
};

class D3D12ConstantBufferView : public RHIDescriptor
{
public:
public:
    D3D12ConstantBufferView(D3D12Device* pDevice, RHIBuffer* buffer, const RHIConstantBufferViewDesc& desc, const std::string& name);
    ~D3D12ConstantBufferView();

    virtual void* GetHandle() const override { return m_pBuffer->GetHandle(); }
    virtual uint32_t GetIndex() const override { return m_descriptor.index; }

    bool Create();
private:
    RHIBuffer* m_pBuffer = nullptr;
    RHIConstantBufferViewDesc m_desc = {};
    D3D12Descriptor m_descriptor;
};

class D3D12Sampler : public RHIDescriptor
{
public:
    D3D12Sampler(D3D12Device* pDevice, const RHISamplerDesc& desc, const std::string& name);
    ~D3D12Sampler();

    virtual void* GetHandle() const override { return nullptr; }
    virtual uint32_t GetIndex() const override { return m_descriptor.index; }

    bool Create();
private:
    RHISamplerDesc m_desc;
    D3D12Descriptor m_descriptor;
};