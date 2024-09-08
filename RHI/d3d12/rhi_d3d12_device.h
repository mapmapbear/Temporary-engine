#pragma once

#include "../rhi_device.h"
#include "RHI/rhi_buffer.h"
#include "directx/d3d12.h"
#include "rhi_d3d12_header.h"

#include <memory>
#include <queue>
#include <vcruntime.h>

namespace D3D12MA {
class Allocator;
class Allocation;
} // namespace D3D12MA

class D3D12DescriptorAllocator {
public:
  D3D12DescriptorAllocator(ID3D12Device *device,
                           D3D12_DESCRIPTOR_HEAP_TYPE type,
                           uint32_t descriptor_count, const std::string &name);
  ~D3D12DescriptorAllocator();

  D3D12Descriptor Allocate();
  void Free(const D3D12Descriptor &descriptor);

  ID3D12DescriptorHeap *GetHeap() const { return m_pHeap; }
  D3D12Descriptor GetDescriptor(uint32_t index) const;

private:
  ID3D12DescriptorHeap *m_pHeap = nullptr;
  uint32_t m_descriptorSize = 0;
  uint32_t m_descirptorCount = 0;
  uint32_t m_allocatedCount = 0;
  bool m_bShaderVisible = false;
  std::vector<D3D12Descriptor> m_freeDescriptors;
};

class D3D12Device;
class D3D12ConstantBufferAllocator {
public:
  D3D12ConstantBufferAllocator(D3D12Device *device, uint32_t buffer_size,
                               const std::string &name);
  void Allocate(uint32_t size, void **cpu_address, uint64_t *gpu_address);
  void Reset();

private:
  std::unique_ptr<RHIBuffer> m_pBuffer;
  uint32_t m_allcatedSize = 0;
};

class D3D12Device : public RHIDevice {
public:
  D3D12Device(const RHIDeviceDesc &desc);
  ~D3D12Device();

  virtual void *GetHandle() const override { return m_pDevice; }

  virtual RHIBuffer *CreateBuffer(const RHIBufferDesc &desc,
                                  const std::string &name) override;
  virtual RHITexture *CreateTexture(const RHITextureDesc &desc,
                                    const std::string &name) override;
  virtual RHIFence *CreateFence(const std::string &name) override;
  virtual RHISwapchain *CreateSwapchain(const RHISwapchainDesc &desc,
                                        const std::string &name) override;
  virtual RHICommandList *CreateCommandList(RHICommandQueue queue_type,
                                            const std::string &name) override;
  virtual RHIShader *CreateShader(const RHIShaderDesc &desc,
                                  const std::vector<uint8_t> &data,
                                  const std::string &name) override;
  virtual RHIPipelineState *
  CreateGraphicsPipelineState(const RHIGraphicsPipelineDesc &desc,
                              const std::string &name) override;
  virtual RHIPipelineState *
  CreateMeshShadingPipelineState(const RHIMeshShadingPipelineDesc &desc,
                                 const std::string &name) override;
  virtual uint32_t
  CreateShaderResourceView(RHIResource *resource,
                           const RHIShaderResourceViewDesc &desc) override;
  virtual uint32_t
  CreateUnorderedAccessView(RHIResource *resource,
                            const RHIUnorderedAccessViewDesc &desc) override;
  virtual uint32_t
  CreateConstantBufferView(RHIBuffer *buffer,
                           const RHIConstantBufferViewDesc &desc) override;
  virtual uint32_t CreateSampler(const RHISamplerDesc &desc) override;
  virtual void ReleaseResourceDescriptor(uint32_t index) override;
  virtual void ReleaseSamplerDescriptor(uint32_t index) override;
  virtual void BeginFrame() override;
  virtual void EndFrame() override;
  virtual uint64_t GetFrameID() const override { return m_nFrameID; }

  bool Init();
  IDXGIFactory4 *GetDxgiFactory4() const { return m_pDxgiFactory; }
  ID3D12CommandQueue *GetGraphicsQueue() const { return m_pGraphicsQueue; }
  ID3D12CommandQueue *GetCopyQueue() const { return m_pCopyQueue; }
  D3D12MA::Allocator *GetResourceAllocator() const {
    return m_pResourceAllocator;
  }
  ID3D12DescriptorHeap *GetResourceDescriptorHeap() const {
    return m_pResDescriptorAllocator->GetHeap();
  }
  ID3D12DescriptorHeap *GetSamplerDescriptorHeap() const {
    return m_pSamplerAllocator->GetHeap();
  }
  ID3D12RootSignature *GetRootSignature() const { return m_pRootSignature; }

  D3D12_GPU_VIRTUAL_ADDRESS AllocateConstantBuffer(void *data,
                                                   size_t data_size);

  void FlushDeferredDeletions();
  void Delete(IUnknown *object);
  void Delete(D3D12MA::Allocation *allocation);

  D3D12Descriptor AllocateRTV();
  D3D12Descriptor AllocateDSV();
  D3D12Descriptor AllocateResourceDescriptor();
  D3D12Descriptor AllocateSampler();

  void DeleteRTV(const D3D12Descriptor &descriptor);
  void DeleteDSV(const D3D12Descriptor &descriptor);
  void DeleteResourceDescriptor(const D3D12Descriptor &descriptor);
  void DeleteSampler(const D3D12Descriptor &descriptor);

private:
  void DoDeferredDeletion(bool force_delete = false);
  void CreateRootSignature();

private:
  RHIDeviceDesc m_desc;

  IDXGIFactory4 *m_pDxgiFactory = nullptr;
  IDXGIAdapter1 *m_pDxgiAdapter = nullptr;

  ID3D12Device *m_pDevice = nullptr;
  ID3D12CommandQueue *m_pGraphicsQueue = nullptr;
  ID3D12CommandQueue *m_pCopyQueue = nullptr;
  ID3D12RootSignature *m_pRootSignature = nullptr;

  D3D12MA::Allocator *m_pResourceAllocator = nullptr;
  static const uint32_t CB_ALLOCATOR_COUNT = 3;
  std::unique_ptr<D3D12ConstantBufferAllocator>
      m_pConstantBufferAllocators[CB_ALLOCATOR_COUNT];

  std::unique_ptr<D3D12DescriptorAllocator> m_pRTVAllocator;
  std::unique_ptr<D3D12DescriptorAllocator> m_pDSVAllocator;
  std::unique_ptr<D3D12DescriptorAllocator> m_pResDescriptorAllocator;
  std::unique_ptr<D3D12DescriptorAllocator> m_pSamplerAllocator;

  uint64_t m_nFrameID = 0;

  struct ObjectDeletion {
    IUnknown *object;
    uint64_t frame;
  };
  std::queue<ObjectDeletion> m_deletionQueue;

  struct AllocationDeletion {
    D3D12MA::Allocation *allocation;
    uint64_t frame;
  };
  std::queue<AllocationDeletion> m_allocationDeletionQueue;

  struct DescriptorDeletion {
    D3D12Descriptor descriptor;
    uint64_t frame;
  };
  std::queue<DescriptorDeletion> m_rtvDeletionQueue;
  std::queue<DescriptorDeletion> m_dsvDeletionQueue;
  std::queue<DescriptorDeletion> m_resourceDeletionQueue;
  std::queue<DescriptorDeletion> m_samplerDeletionQueue;
};