#pragma once

#include "rhi_resource.h"

class RHIFence : public RHIResource
{
public:
	virtual ~RHIFence() {}

	virtual void Wait(uint64_t value) = 0;
	virtual void Signal(uint64_t value) = 0;
};