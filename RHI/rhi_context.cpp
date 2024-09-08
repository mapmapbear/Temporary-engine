#include "rhi_context.h"
#include "d3d12/rhi_d3d12_device.h"

RHIDevice* CreateGfxDevice(const RHIDeviceDesc& desc)
{
	RHIDevice* pDevice = nullptr;

	switch (desc.backend)
	{
	case RHIRenderBackend::D3D12:
		pDevice = new D3D12Device(desc);
		((D3D12Device*)pDevice)->Init();
		break;
	default:
		break;
	}
	
	return pDevice;
}
