#pragma once

#include "rhi_d3d12_header.h"
#include "../rhi_pipeline_state.h"

class D3D12Device;

class D3D12GraphicsPipelineState : public RHIPipelineState
{
public:
    D3D12GraphicsPipelineState(D3D12Device* pDevice, const RHIGraphicsPipelineDesc& desc, const std::string& name);
    ~D3D12GraphicsPipelineState();

    virtual void* GetHandle() const { return m_pPipelineState; }
    bool Create();

    D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return m_primitiveTopology; }

private:
    ID3D12PipelineState* m_pPipelineState = nullptr;
    RHIGraphicsPipelineDesc m_desc;
    D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

class D3D12MeshShadingPipelineState : public RHIPipelineState
{
};