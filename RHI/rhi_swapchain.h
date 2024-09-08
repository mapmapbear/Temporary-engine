#pragma once

#include "rhi_resource.h"

class RHITexture;

class RHISwapchain : public RHIResource
{
public:
	virtual ~RHISwapchain() {}

	virtual bool Present() = 0;
	virtual bool Resize(uint32_t width, uint32_t height) = 0;
	virtual RHITexture* GetBackBuffer() const = 0;

	const RHISwapchainDesc& GetDesc() const { return m_desc; }

protected:
	RHISwapchainDesc m_desc = {};
};