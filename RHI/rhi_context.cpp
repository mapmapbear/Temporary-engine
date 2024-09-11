#include "rhi_context.h"
#include "d3d12/rhi_d3d12_device.h"
#include "../utils/fassert.h"

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

uint32_t GetFormatRowPitch(RHIFormat format, uint32_t width)
{
    switch (format)
    {
    case RHIFormat::RGBA32F:
    case RHIFormat::RGBA32UI:
    case RHIFormat::RGBA32SI:
        return width * 16;
    case RHIFormat::RGBA16F:
    case RHIFormat::RGBA16UI:
    case RHIFormat::RGBA16SI:
    case RHIFormat::RGBA16UNORM:
    case RHIFormat::RGBA16SNORM:
        return width * 8;
    case RHIFormat::RGBA8UI:
    case RHIFormat::RGBA8SI:
    case RHIFormat::RGBA8UNORM:
    case RHIFormat::RGBA8SNORM:
    case RHIFormat::RGBA8SRGB:
        return width * 4;
    case RHIFormat::RG32F:
    case RHIFormat::RG32UI:
    case RHIFormat::RG32SI:
        return width * 8;
    case RHIFormat::RG16F:
    case RHIFormat::RG16UI:
    case RHIFormat::RG16SI:
    case RHIFormat::RG16UNORM:
    case RHIFormat::RG16SNORM:
        return width * 4;
    case RHIFormat::RG8UI:
    case RHIFormat::RG8SI:
    case RHIFormat::RG8UNORM:
    case RHIFormat::RG8SNORM:
        return width * 2;
    case RHIFormat::R32F:
    case RHIFormat::R32UI:
    case RHIFormat::R32SI:
        return width * 4;
    case RHIFormat::R16F:
    case RHIFormat::R16UI:
    case RHIFormat::R16SI:
    case RHIFormat::R16UNORM:
    case RHIFormat::R16SNORM:
        return width * 2;
    case RHIFormat::R8UI:
    case RHIFormat::R8SI:
    case RHIFormat::R8UNORM:
    case RHIFormat::R8SNORM:
        return width;
    default:
        RE_ASSERT(false);
        return 0;
    }
}

uint32_t GetFormatBlockWidth(RHIFormat format)
{
    //todo : compressed formats
    RE_ASSERT(format <= RHIFormat::R8SNORM);
    return 1;
}

uint32_t GetFormatBlockHeight(RHIFormat format)
{
    //todo : compressed formats
    RE_ASSERT(format <= RHIFormat::R8SNORM);
    return 1;
}