#pragma once

#include "rhi_resource.h"

class RHIPipelineState : public RHIResource
{
public:
    RHIPipelineType GetType() const { return m_type; }

protected:
    RHIPipelineType m_type;
};