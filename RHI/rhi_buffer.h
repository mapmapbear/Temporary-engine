#pragma once

#include "rhi_resource.h"

class RHIBuffer : public RHIResource
{
public:
	const RHIBufferDesc& GetDesc() const { return m_desc; }

	virtual void* GetCpuAddress() = 0;
	virtual uint64_t GetGpuAddress() = 0;

protected:
	RHIBufferDesc m_desc = {};
};
