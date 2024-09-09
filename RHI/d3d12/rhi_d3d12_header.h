#pragma once

#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"
#include "../rhi_define.h"
#include <locale>
#include <codecvt>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p){p->Release(); p = nullptr;}
#endif

struct D3D12Descriptor
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = {};
	uint32_t index = GFX_INVALID_RESOURCE;
};

inline bool IsNullDescriptor(const D3D12Descriptor& descriptor)
{
	return descriptor.cpu_handle.ptr == 0 && descriptor.gpu_handle.ptr == 0;
}

inline std::wstring string_to_wstring(std::string s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(s);
}


inline D3D12_HEAP_TYPE d3d12_heap_type(RHIMemoryType memory_type)
{
	switch (memory_type)
	{
	case RHIMemoryType::GpuOnly:
		return D3D12_HEAP_TYPE_DEFAULT;
	case RHIMemoryType::CpuOnly:
	case RHIMemoryType::CpuToGpu:
		return D3D12_HEAP_TYPE_UPLOAD;
	case RHIMemoryType::GpuToCpu:
		return D3D12_HEAP_TYPE_READBACK;
	default:
		return D3D12_HEAP_TYPE_DEFAULT;
	}
}

inline D3D12_RESOURCE_STATES d3d12_resource_state(RHIResourceState state)
{
	switch (state)
	{
	case RHIResourceState::Common:
		return D3D12_RESOURCE_STATE_COMMON;
	case RHIResourceState::RenderTarget:
		return D3D12_RESOURCE_STATE_RENDER_TARGET;
	case RHIResourceState::UnorderedAccess:
		return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	case RHIResourceState::DepthStencil:
		return D3D12_RESOURCE_STATE_DEPTH_WRITE;
	case RHIResourceState::DepthStencilReadOnly:
		return D3D12_RESOURCE_STATE_DEPTH_READ;
	case RHIResourceState::ShaderResource:
		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	case RHIResourceState::ShaderResourcePSOnly:
		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	case RHIResourceState::IndirectArg:
		return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
	case RHIResourceState::CopyDst:
		return D3D12_RESOURCE_STATE_COPY_DEST;
	case RHIResourceState::CopySrc:
		return D3D12_RESOURCE_STATE_COPY_SOURCE;
	case RHIResourceState::ResolveDst:
		return D3D12_RESOURCE_STATE_RESOLVE_DEST;
	case RHIResourceState::ResolveSrc:
		return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
	case RHIResourceState::Present:
		return D3D12_RESOURCE_STATE_PRESENT;
	default:
		return D3D12_RESOURCE_STATE_COMMON;
	}
}

inline DXGI_FORMAT dxgi_format(RHIFormat format)
{
	switch (format)
	{
	case RHIFormat::Unknown:
		return DXGI_FORMAT_UNKNOWN;
	case RHIFormat::RGBA32F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case RHIFormat::RGBA32UI:
		return DXGI_FORMAT_R32G32B32A32_UINT;
	case RHIFormat::RGBA32SI:
		return DXGI_FORMAT_R32G32B32A32_SINT;
	case RHIFormat::RGBA16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RHIFormat::RGBA16UI:
		return DXGI_FORMAT_R16G16B16A16_UINT;
	case RHIFormat::RGBA16SI:
		return DXGI_FORMAT_R16G16B16A16_SINT;
	case RHIFormat::RGBA16UNORM:
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case RHIFormat::RGBA16SNORM:
		return DXGI_FORMAT_R16G16B16A16_SNORM;
	case RHIFormat::RGBA8UI:
		return DXGI_FORMAT_R8G8B8A8_UINT;
	case RHIFormat::RGBA8SI:
		return DXGI_FORMAT_R8G8B8A8_SINT;
	case RHIFormat::RGBA8UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case RHIFormat::RGBA8SNORM:
		return DXGI_FORMAT_R8G8B8A8_SNORM;
	case RHIFormat::RGBA8SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case RHIFormat::RG32F:
		return DXGI_FORMAT_R32G32_FLOAT;
	case RHIFormat::RG32UI:
		return DXGI_FORMAT_R32G32_UINT;
	case RHIFormat::RG32SI:
		return DXGI_FORMAT_R32G32_SINT;
	case RHIFormat::RG16F:
		return DXGI_FORMAT_R16G16_FLOAT;
	case RHIFormat::RG16UI:
		return DXGI_FORMAT_R16G16_UINT;
	case RHIFormat::RG16SI:
		return DXGI_FORMAT_R16G16_SINT;
	case RHIFormat::RG16UNORM:
		return DXGI_FORMAT_R16G16_UNORM;
	case RHIFormat::RG16SNORM:
		return DXGI_FORMAT_R16G16_SNORM;
	case RHIFormat::RG8UI:
		return DXGI_FORMAT_R8G8_UINT;
	case RHIFormat::RG8SI:
		return DXGI_FORMAT_R8G8_SINT;
	case RHIFormat::RG8UNORM:
		return DXGI_FORMAT_R8G8_UNORM;
	case RHIFormat::RG8SNORM:
		return DXGI_FORMAT_R8G8_SNORM;
	case RHIFormat::R32F:
		return DXGI_FORMAT_R32_FLOAT;
	case RHIFormat::R32UI:
		return DXGI_FORMAT_R32_UINT;
	case RHIFormat::R32SI:
		return DXGI_FORMAT_R32_SINT;
	case RHIFormat::R16F:
		return DXGI_FORMAT_R16_FLOAT;
	case RHIFormat::R16UI:
		return DXGI_FORMAT_R16_UINT;
	case RHIFormat::R16SI:
		return DXGI_FORMAT_R16_SINT;
	case RHIFormat::R16UNORM:
		return DXGI_FORMAT_R16_UNORM;
	case RHIFormat::R16SNORM:
		return DXGI_FORMAT_R16_SNORM;
	case RHIFormat::R8UI:
		return DXGI_FORMAT_R8_UINT;
	case RHIFormat::R8SI:
		return DXGI_FORMAT_R8_SINT;
	case RHIFormat::R8UNORM:
		return DXGI_FORMAT_R8_UNORM;
	case RHIFormat::R8SNORM:
		return DXGI_FORMAT_R8_SNORM;
	case RHIFormat::D32F:
		return DXGI_FORMAT_D32_FLOAT;
	case RHIFormat::D32FS8:
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case RHIFormat::D16:
		return DXGI_FORMAT_D16_UNORM;
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

inline D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE d3d12_render_pass_loadop(RHIRenderPassLoadOp loadOp)
{
	switch (loadOp)
	{
	case RHIRenderPassLoadOp::Load:
		return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
	case RHIRenderPassLoadOp::Clear:
		return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
	case RHIRenderPassLoadOp::DontCare:
		return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
	default:
		return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
	}
}

inline D3D12_RENDER_PASS_ENDING_ACCESS_TYPE d3d12_render_pass_storeop(RHIRenderPassStoreOp storeOp)
{
	switch (storeOp)
	{
	case RHIRenderPassStoreOp::Store:
		return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
	case RHIRenderPassStoreOp::DontCare:
		return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
	default:
		return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
	}
}

inline D3D12_BLEND d3d12_blend(RHIBlendFactor blend_factor)
{
	switch (blend_factor)
	{
	case RHIBlendFactor::Zero:
		return D3D12_BLEND_ZERO;
	case RHIBlendFactor::One:
		return D3D12_BLEND_ONE;
	case RHIBlendFactor::SrcColor:
		return D3D12_BLEND_SRC_COLOR;
	case RHIBlendFactor::InvSrcColor:
		return D3D12_BLEND_INV_SRC_COLOR;
	case RHIBlendFactor::SrcAlpha:
		return D3D12_BLEND_SRC_ALPHA;
	case RHIBlendFactor::InvSrcAlpha:
		return D3D12_BLEND_INV_SRC_ALPHA;
	case RHIBlendFactor::DstAlpha:
		return D3D12_BLEND_DEST_ALPHA;
	case RHIBlendFactor::InvDstAlpha:
		return D3D12_BLEND_INV_DEST_ALPHA;
	case RHIBlendFactor::DstColor:
		return D3D12_BLEND_DEST_COLOR;
	case RHIBlendFactor::InvDstColor:
		return D3D12_BLEND_INV_DEST_COLOR;
	case RHIBlendFactor::SrcAlphaClamp:
		return D3D12_BLEND_SRC_ALPHA_SAT;
	case RHIBlendFactor::ConstantFactor:
		return D3D12_BLEND_BLEND_FACTOR;
	case RHIBlendFactor::InvConstantFactor:
		return D3D12_BLEND_INV_BLEND_FACTOR;
	default:
		return D3D12_BLEND_ONE;
	}
}

inline D3D12_BLEND_OP d3d12_blend_op(RHIBlendOp blend_op)
{
	switch (blend_op)
	{
	case RHIBlendOp::Add:
		return D3D12_BLEND_OP_ADD;
	case RHIBlendOp::Subtract:
		return D3D12_BLEND_OP_SUBTRACT;
	case RHIBlendOp::ReverseSubtract:
		return D3D12_BLEND_OP_REV_SUBTRACT;
	case RHIBlendOp::Min:
		return D3D12_BLEND_OP_MIN;
	case RHIBlendOp::Max:
		return D3D12_BLEND_OP_MAX;
	default:
		return D3D12_BLEND_OP_ADD;
	}
}

inline D3D12_RENDER_TARGET_BLEND_DESC d3d12_rt_blend_desc(const RHIBlendState& blendState)
{
	D3D12_RENDER_TARGET_BLEND_DESC desc = {};
	desc.BlendEnable = blendState.blend_enable;
	desc.SrcBlend = d3d12_blend(blendState.color_src);
	desc.DestBlend = d3d12_blend(blendState.color_dst);
	desc.BlendOp = d3d12_blend_op(blendState.color_op);
	desc.SrcBlendAlpha = d3d12_blend(blendState.alpha_src);
	desc.DestBlendAlpha = d3d12_blend(blendState.alpha_dst);
	desc.BlendOpAlpha = d3d12_blend_op(blendState.alpha_op);
	desc.RenderTargetWriteMask = blendState.write_mask;

	return desc;
}

inline D3D12_BLEND_DESC d3d12_blend_desc(const RHIBlendState* blendStates)
{
	D3D12_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = true;

	for (int i = 0; i < 8; ++i)
	{
		desc.RenderTarget[i] = d3d12_rt_blend_desc(blendStates[i]);
	}

	return desc;
}

inline D3D12_CULL_MODE d3d12_cull_mode(RHICullMode cull_mode)
{
	switch (cull_mode)
	{
	case RHICullMode::None:
		return D3D12_CULL_MODE_NONE;
	case RHICullMode::Front:
		return D3D12_CULL_MODE_FRONT;
	case RHICullMode::Back:
		return D3D12_CULL_MODE_BACK;
	default:
		return D3D12_CULL_MODE_NONE;
	}
}

inline D3D12_RASTERIZER_DESC d3d12_rasterizer_desc(const RHIRasterizerState& rasterizerState)
{
	D3D12_RASTERIZER_DESC desc = {};
	desc.FillMode = rasterizerState.wireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	desc.CullMode = d3d12_cull_mode(rasterizerState.cull_mode);
	desc.FrontCounterClockwise = rasterizerState.front_ccw;
	desc.DepthBias = (INT)rasterizerState.depth_bias;
	desc.DepthBiasClamp = rasterizerState.depth_bias_clamp;
	desc.SlopeScaledDepthBias = rasterizerState.depth_slope_scale;
	desc.DepthClipEnable = rasterizerState.depth_clip;
	desc.AntialiasedLineEnable = rasterizerState.line_aa;
	desc.ConservativeRaster = rasterizerState.conservative_raster ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return desc;
}

inline D3D12_COMPARISON_FUNC d3d12_compare_func(RHICompareFunc func)
{
	switch (func)
	{
	case RHICompareFunc::Never:
		return D3D12_COMPARISON_FUNC_NEVER;
	case RHICompareFunc::Less:
		return D3D12_COMPARISON_FUNC_LESS;
	case RHICompareFunc::Equal:
		return D3D12_COMPARISON_FUNC_EQUAL;
	case RHICompareFunc::LessEqual:
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case RHICompareFunc::Greater:
		return D3D12_COMPARISON_FUNC_GREATER;
	case RHICompareFunc::NotEqual:
		return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case RHICompareFunc::GreaterEqual:
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case RHICompareFunc::Always:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	default:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

inline D3D12_STENCIL_OP d3d12_stencil_op(RHIStencilOp stencil_op)
{
	switch (stencil_op)
	{
	case RHIStencilOp::Keep:
		return D3D12_STENCIL_OP_KEEP;
	case RHIStencilOp::Zero:
		return D3D12_STENCIL_OP_ZERO;
	case RHIStencilOp::Replace:
		return D3D12_STENCIL_OP_REPLACE;
	case RHIStencilOp::IncreaseClamp:
		return D3D12_STENCIL_OP_INCR_SAT;
	case RHIStencilOp::DecreaseClamp:
		return D3D12_STENCIL_OP_DECR_SAT;
	case RHIStencilOp::Invert:
		return D3D12_STENCIL_OP_INVERT;
	case RHIStencilOp::IncreaseWrap:
		return D3D12_STENCIL_OP_INCR;
	case RHIStencilOp::DecreaseWrap:
		return D3D12_STENCIL_OP_DECR;
	default:
		return D3D12_STENCIL_OP_KEEP;
	}
}

inline D3D12_DEPTH_STENCILOP_DESC d3d12_depth_stencil_op(const RHIDepthStencilOp& depthStencilOp)
{
	D3D12_DEPTH_STENCILOP_DESC desc = {};
	desc.StencilFailOp = d3d12_stencil_op(depthStencilOp.stencil_fail);
	desc.StencilDepthFailOp = d3d12_stencil_op(depthStencilOp.depth_fail);
	desc.StencilPassOp = d3d12_stencil_op(depthStencilOp.pass);
	desc.StencilFunc = d3d12_compare_func(depthStencilOp.stencil_func);

	return desc;
}

inline D3D12_DEPTH_STENCIL_DESC d3d12_depth_stencil_desc(const RHIDepthStencilState& depthStencilState)
{
	D3D12_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = depthStencilState.depth_test;
	desc.DepthWriteMask = depthStencilState.depth_write ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = d3d12_compare_func(depthStencilState.depth_func);
	desc.StencilEnable = depthStencilState.stencil_test;
	desc.StencilReadMask = depthStencilState.stencil_read_mask;
	desc.StencilWriteMask = depthStencilState.stencil_write_mask;
	desc.FrontFace = d3d12_depth_stencil_op(depthStencilState.front);
	desc.BackFace = d3d12_depth_stencil_op(depthStencilState.back);

	return desc;
}

inline D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d12_topology_type(RHIPrimitiveType primitive_type)
{
	switch (primitive_type)
	{
	case RHIPrimitiveType::PointList:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case RHIPrimitiveType::LineList:
	case RHIPrimitiveType::LineStrip:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case RHIPrimitiveType::TriangleList:
	case RHIPrimitiveType::TriangleTrip:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}

inline D3D_PRIMITIVE_TOPOLOGY d3d12_primitive_topology(RHIPrimitiveType primitive_type)
{
	switch (primitive_type)
	{
	case RHIPrimitiveType::PointList:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case RHIPrimitiveType::LineList:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case RHIPrimitiveType::LineStrip:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case RHIPrimitiveType::TriangleList:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case RHIPrimitiveType::TriangleTrip:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default:
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

inline D3D12_FILTER_TYPE d3d12_filter_type(RHIFilter filter)
{
	switch (filter)
	{
	case RHIFilter::Point:
		return D3D12_FILTER_TYPE_POINT;
	case RHIFilter::Linear:
		return D3D12_FILTER_TYPE_LINEAR;
	default:
		return D3D12_FILTER_TYPE_POINT;
	}
}

inline D3D12_FILTER d3d12_filter(const RHISamplerDesc& desc)
{
	D3D12_FILTER_REDUCTION_TYPE reduction = desc.enable_compare ? D3D12_FILTER_REDUCTION_TYPE_COMPARISON : D3D12_FILTER_REDUCTION_TYPE_STANDARD;

	if (desc.enable_anisotropy)
	{
		return D3D12_ENCODE_ANISOTROPIC_FILTER(reduction);
	}
	else
	{
		D3D12_FILTER_TYPE min = d3d12_filter_type(desc.min_filter);
		D3D12_FILTER_TYPE mag = d3d12_filter_type(desc.mag_filter);
		D3D12_FILTER_TYPE mip = d3d12_filter_type(desc.mip_filter);

		return D3D12_ENCODE_BASIC_FILTER(min, mag, mip, reduction);
	}
}

inline D3D12_TEXTURE_ADDRESS_MODE d3d12_address_mode(RHISamplerAddressMode mode)
{
	switch (mode)
	{
	case RHISamplerAddressMode::Repeat:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case RHISamplerAddressMode::MirroredRepeat:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case RHISamplerAddressMode::ClampToEdge:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case RHISamplerAddressMode::ClampToBorder:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	default:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}
}