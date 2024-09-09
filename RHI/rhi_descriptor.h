#pragma once

#include "rhi_resource.h"

class RHIDescriptor : public RHIResource
{
public:
    virtual uint32_t GetIndex() const = 0;
};