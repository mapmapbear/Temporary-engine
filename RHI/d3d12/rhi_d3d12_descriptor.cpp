#include "rhi_d3d12_descriptor.h"
#include "../utils/fassert.h"
#include "rhi_d3d12_buffer.h"
#include "rhi_d3d12_device.h"
#include "rhi_d3d12_texture.h"


D3D12ShaderResourceView::D3D12ShaderResourceView(
    D3D12Device *pDevice, RHIResource *pResource,
    const RHIShaderResourceViewDesc &desc, const std::string &name) {
  m_pDevice = pDevice;
  m_name = name;
  m_pResource = pResource;
  m_desc = desc;
}

D3D12ShaderResourceView::~D3D12ShaderResourceView() {
  ((D3D12Device *)m_pDevice)->DeleteResourceDescriptor(m_descriptor);
}

bool D3D12ShaderResourceView::Create() {
  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

  switch (m_desc.type) {
  case RHIShaderResourceViewType::Texture2D: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageShaderResource);

    srvDesc.Format = dxgi_format(textureDesc.format);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = m_desc.texture.mip_slice;
    srvDesc.Texture2D.MipLevels = m_desc.texture.mip_levels;
    srvDesc.Texture2D.PlaneSlice = m_desc.texture.plane_slice;
    break;
  }
  case RHIShaderResourceViewType::Texture2DArray: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageShaderResource);

    srvDesc.Format = dxgi_format(textureDesc.format);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = m_desc.texture.mip_slice;
    srvDesc.Texture2DArray.MipLevels = m_desc.texture.mip_levels;
    srvDesc.Texture2DArray.FirstArraySlice = m_desc.texture.array_slice;
    srvDesc.Texture2DArray.ArraySize = m_desc.texture.array_size;
    srvDesc.Texture2DArray.PlaneSlice = m_desc.texture.plane_slice;
    break;
  }
  case RHIShaderResourceViewType::Texture3D: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageShaderResource);

    srvDesc.Format = dxgi_format(textureDesc.format);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
    srvDesc.Texture3D.MostDetailedMip = m_desc.texture.mip_slice;
    srvDesc.Texture3D.MipLevels = m_desc.texture.mip_levels;
    break;
  }
  case RHIShaderResourceViewType::TextureCube: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageShaderResource);

    srvDesc.Format = dxgi_format(textureDesc.format);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MostDetailedMip = m_desc.texture.mip_slice;
    srvDesc.TextureCube.MipLevels = m_desc.texture.mip_levels;
    break;
  }
  case RHIShaderResourceViewType::TextureCubeArray: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageShaderResource);

    srvDesc.Format = dxgi_format(textureDesc.format);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
    srvDesc.TextureCubeArray.MostDetailedMip = m_desc.texture.mip_slice;
    srvDesc.TextureCubeArray.MipLevels = m_desc.texture.mip_levels;
    srvDesc.TextureCubeArray.First2DArrayFace = 0;
    srvDesc.TextureCubeArray.NumCubes = m_desc.texture.array_size / 6;
    break;
  }
  case RHIShaderResourceViewType::StructuredBuffer: {
    const RHIBufferDesc &bufferDesc = ((RHIBuffer *)m_pResource)->GetDesc();
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageStructuredBuffer);
    RE_ASSERT(m_desc.buffer.offset % bufferDesc.stride == 0);
    RE_ASSERT(m_desc.buffer.size % bufferDesc.stride == 0);

    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = m_desc.buffer.offset / bufferDesc.stride;
    srvDesc.Buffer.NumElements = m_desc.buffer.size / bufferDesc.stride;
    srvDesc.Buffer.StructureByteStride = bufferDesc.stride;
    break;
  }
  case RHIShaderResourceViewType::TypedBuffer: {
    const RHIBufferDesc &bufferDesc = ((RHIBuffer *)m_pResource)->GetDesc();
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageTypedBuffer);
    RE_ASSERT(m_desc.buffer.offset % bufferDesc.stride == 0);
    RE_ASSERT(m_desc.buffer.size % bufferDesc.stride == 0);

    srvDesc.Format = dxgi_format(bufferDesc.format);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = m_desc.buffer.offset / bufferDesc.stride;
    srvDesc.Buffer.NumElements = m_desc.buffer.size / bufferDesc.stride;
  }
  case RHIShaderResourceViewType::RawBuffer: {
    const RHIBufferDesc &bufferDesc = ((RHIBuffer *)m_pResource)->GetDesc();
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageRawBuffer);
    RE_ASSERT(bufferDesc.stride == 4);
    RE_ASSERT(m_desc.buffer.offset % 4 == 0);
    RE_ASSERT(m_desc.buffer.size % 4 == 0);

    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = m_desc.buffer.offset / 4;
    srvDesc.Buffer.NumElements = m_desc.buffer.size / 4;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
    break;
  }
  default:
    break;
  }

  m_descriptor = ((D3D12Device *)m_pDevice)->AllocateResourceDescriptor();

  ID3D12Device *pDevice = (ID3D12Device *)m_pDevice->GetHandle();
  pDevice->CreateShaderResourceView((ID3D12Resource *)m_pResource->GetHandle(),
                                    &srvDesc, m_descriptor.cpu_handle);

  return true;
}

D3D12UnorderedAccessView::D3D12UnorderedAccessView(
    D3D12Device *pDevice, RHIResource *pResource,
    const RHIUnorderedAccessViewDesc &desc, const std::string &name) {
  m_pDevice = pDevice;
  m_name = name;
  m_pResource = pResource;
  m_desc = desc;
}

D3D12UnorderedAccessView::~D3D12UnorderedAccessView() {
  ((D3D12Device *)m_pDevice)->DeleteResourceDescriptor(m_descriptor);
}

bool D3D12UnorderedAccessView::Create() {
  D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

  switch (m_desc.type) {
  case RHIUnorderedAccessViewType::Texture2D: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageUnorderedAccess);

    uavDesc.Format = dxgi_format(textureDesc.format);
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = m_desc.texture.mip_slice;
    uavDesc.Texture2D.PlaneSlice = m_desc.texture.plane_slice;
    break;
  }
  case RHIUnorderedAccessViewType::Texture2DArray: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageUnorderedAccess);

    uavDesc.Format = dxgi_format(textureDesc.format);
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    uavDesc.Texture2DArray.MipSlice = m_desc.texture.mip_slice;
    uavDesc.Texture2DArray.FirstArraySlice = m_desc.texture.array_slice;
    uavDesc.Texture2DArray.ArraySize = m_desc.texture.array_size;
    uavDesc.Texture2DArray.PlaneSlice = m_desc.texture.plane_slice;
    break;
  }
  case RHIUnorderedAccessViewType::Texture3D: {
    const RHITextureDesc &textureDesc = ((RHITexture *)m_pResource)->GetDesc();
    RE_ASSERT(textureDesc.usage & GfxTextureUsageUnorderedAccess);

    uavDesc.Format = dxgi_format(textureDesc.format);
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
    uavDesc.Texture3D.MipSlice = m_desc.texture.mip_slice;
    uavDesc.Texture3D.FirstWSlice = 0;
    uavDesc.Texture3D.WSize = textureDesc.depth;
    break;
  }
  case RHIUnorderedAccessViewType::StructuredBuffer: {
    const RHIBufferDesc &bufferDesc = ((RHIBuffer *)m_pResource)->GetDesc();
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageStructuredBuffer);
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageUnorderedAccess);
    RE_ASSERT(m_desc.buffer.offset % bufferDesc.stride == 0);
    RE_ASSERT(m_desc.buffer.size % bufferDesc.stride == 0);

    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = m_desc.buffer.offset / bufferDesc.stride;
    uavDesc.Buffer.NumElements = m_desc.buffer.size / bufferDesc.stride;
    uavDesc.Buffer.StructureByteStride = bufferDesc.stride;
    break;
  }
  case RHIUnorderedAccessViewType::TypedBuffer: {
    const RHIBufferDesc &bufferDesc = ((RHIBuffer *)m_pResource)->GetDesc();
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageTypedBuffer);
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageUnorderedAccess);
    RE_ASSERT(m_desc.buffer.offset % bufferDesc.stride == 0);
    RE_ASSERT(m_desc.buffer.size % bufferDesc.stride == 0);

    uavDesc.Format = dxgi_format(bufferDesc.format);
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = m_desc.buffer.offset / bufferDesc.stride;
    uavDesc.Buffer.NumElements = m_desc.buffer.size / bufferDesc.stride;
    break;
  }
  case RHIUnorderedAccessViewType::RawBuffer: {
    const RHIBufferDesc &bufferDesc = ((RHIBuffer *)m_pResource)->GetDesc();
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageRawBuffer);
    RE_ASSERT(bufferDesc.usage & GfxBufferUsageUnorderedAccess);
    RE_ASSERT(bufferDesc.stride == 4);
    RE_ASSERT(m_desc.buffer.offset % 4 == 0);
    RE_ASSERT(m_desc.buffer.size % 4 == 0);

    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = m_desc.buffer.offset / 4;
    uavDesc.Buffer.NumElements = m_desc.buffer.size / 4;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
    break;
  }
  default:
    break;
  }

  m_descriptor = ((D3D12Device *)m_pDevice)->AllocateResourceDescriptor();

  ID3D12Device *pDevice = (ID3D12Device *)m_pDevice->GetHandle();
  pDevice->CreateUnorderedAccessView((ID3D12Resource *)m_pResource->GetHandle(),
                                     nullptr, &uavDesc,
                                     m_descriptor.cpu_handle);

  return true;
}

D3D12ConstantBufferView::D3D12ConstantBufferView(
    D3D12Device *pDevice, RHIBuffer *buffer,
    const RHIConstantBufferViewDesc &desc, const std::string &name) {
  m_pDevice = pDevice;
  m_name = name;
  m_pBuffer = buffer;
  m_desc = desc;
}

D3D12ConstantBufferView::~D3D12ConstantBufferView() {
  ((D3D12Device *)m_pDevice)->DeleteResourceDescriptor(m_descriptor);
}

bool D3D12ConstantBufferView::Create() {
  RE_ASSERT(m_pBuffer->GetDesc().usage & GfxBufferUsageConstantBuffer);

  D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
  cbvDesc.BufferLocation = m_pBuffer->GetGpuAddress() + m_desc.offset;
  cbvDesc.SizeInBytes = m_desc.size;

  m_descriptor = ((D3D12Device *)m_pDevice)->AllocateResourceDescriptor();

  ID3D12Device *pDevice = (ID3D12Device *)m_pDevice->GetHandle();
  pDevice->CreateConstantBufferView(&cbvDesc, m_descriptor.cpu_handle);

  return true;
}

D3D12Sampler::D3D12Sampler(D3D12Device *pDevice, const RHISamplerDesc &desc,
                           const std::string &name) {
  m_pDevice = pDevice;
  m_name = name;
  m_desc = desc;
}

D3D12Sampler::~D3D12Sampler() {
  ((D3D12Device *)m_pDevice)->DeleteSampler(m_descriptor);
}

bool D3D12Sampler::Create() {
  D3D12_SAMPLER_DESC samplerDesc = {};
  samplerDesc.Filter = d3d12_filter(m_desc);
  samplerDesc.AddressU = d3d12_address_mode(m_desc.address_u);
  samplerDesc.AddressV = d3d12_address_mode(m_desc.address_v);
  samplerDesc.AddressW = d3d12_address_mode(m_desc.address_w);
  samplerDesc.MipLODBias = m_desc.mip_bias;
  samplerDesc.MaxAnisotropy = (UINT)m_desc.max_anisotropy;
  samplerDesc.ComparisonFunc = d3d12_compare_func(m_desc.compare_func);
  samplerDesc.MinLOD = m_desc.min_lod;
  samplerDesc.MaxLOD = m_desc.max_lod;
  memcpy(samplerDesc.BorderColor, m_desc.border_color, sizeof(float) * 4);

  m_descriptor = ((D3D12Device *)m_pDevice)->AllocateSampler();

  ID3D12Device *pDevice = (ID3D12Device *)m_pDevice->GetHandle();
  pDevice->CreateSampler(&samplerDesc, m_descriptor.cpu_handle);

  return true;
}
