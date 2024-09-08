#pragma once

#include "rhi_resource.h"

class RHIShader : public RHIResource
{
public:
    const RHIShaderDesc& GetDesc() const { return m_desc; }

    uint64_t GetHash() const { return m_hash; }

protected:
    RHIShaderDesc m_desc = {};
    uint64_t m_hash = 0;
};