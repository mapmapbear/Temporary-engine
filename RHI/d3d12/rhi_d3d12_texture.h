#pragma once

#include "rhi_d3d12_header.h"
#include "../rhi_texture.h"

class D3D12Device;

namespace D3D12MA
{
	class Allocation;
}

class D3D12Texture : public RHITexture
{
public:
	D3D12Texture(D3D12Device* pDevice, const RHITextureDesc& desc, const std::string& name);
	~D3D12Texture();

	virtual void* GetHandle() const override { return m_pTexture; }

	bool Create();
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTV(uint32_t mip_slice, uint32_t array_slice);
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSV(uint32_t mip_slice, uint32_t array_slice);

private:
	ID3D12Resource* m_pTexture = nullptr;
	D3D12MA::Allocation* m_pAllocation = nullptr;

	std::vector<D3D12Descriptor> m_RTV;
	std::vector<D3D12Descriptor> m_DSV;

private:
	friend class D3D12Swapchain;
};
