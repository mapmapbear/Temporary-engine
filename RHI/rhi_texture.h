#pragma once

#include "rhi_resource.h"

class RHITexture : public RHIResource
{
public:
	const RHITextureDesc& GetDesc() const { return m_desc; }

protected:
	RHITextureDesc m_desc = {};
};
