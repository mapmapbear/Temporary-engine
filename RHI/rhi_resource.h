#pragma once

#include "rhi_define.h"
#include <string>

class RHIDevice;

class RHIResource
{
public:
	virtual ~RHIResource() {}

	virtual void* GetHandle() const = 0;

	RHIDevice* GetDevice() const { return m_pDevice; }
	const std::string& GetName() const { return m_name; }

protected:
	RHIDevice* m_pDevice = nullptr;
	std::string m_name;
};