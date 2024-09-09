#pragma once

#include "rhi_define.h"
#include <string>
#include <vector>

class RHIBuffer;
class RHITexture;
class RHIFence;
class RHISwapchain;
class RHICommandList;
class RHIShader;
class RHIPipelineState;
class RHIResource;
class RHIDescriptor;

class RHIDevice {
public:
  virtual ~RHIDevice() {}
  virtual void *GetHandle() const = 0;

  virtual RHISwapchain *CreateSwapchain(const RHISwapchainDesc &desc,
                                        const std::string &name) = 0;
  virtual RHICommandList *CreateCommandList(RHICommandQueue queue_type,
                                            const std::string &name) = 0;
  virtual RHIFence *CreateFence(const std::string &name) = 0;
  virtual RHIBuffer *CreateBuffer(const RHIBufferDesc &desc,
                                  const std::string &name) = 0;
  virtual RHITexture *CreateTexture(const RHITextureDesc &desc,
                                    const std::string &name) = 0;
  virtual RHIShader *CreateShader(const RHIShaderDesc &desc,
                                  const std::vector<uint8_t> &data,
                                  const std::string &name) = 0;
  virtual RHIPipelineState *
  CreateGraphicsPipelineState(const RHIGraphicsPipelineDesc &desc,
                              const std::string &name) = 0;
  virtual RHIPipelineState *
  CreateMeshShadingPipelineState(const RHIMeshShadingPipelineDesc &desc,
                                 const std::string &name) = 0;
  virtual RHIDescriptor *
  CreateShaderResourceView(RHIResource *resource,
                           const RHIShaderResourceViewDesc &desc,
                           const std::string &name) = 0;
  virtual RHIDescriptor *
  CreateUnorderedAccessView(RHIResource *resource,
                            const RHIUnorderedAccessViewDesc &desc,
                            const std::string &name) = 0;
  virtual RHIDescriptor *
  CreateConstantBufferView(RHIBuffer *buffer,
                           const RHIConstantBufferViewDesc &desc,
                           const std::string &name) = 0;
  virtual RHIDescriptor *CreateSampler(const RHISamplerDesc &desc,
                                       const std::string &name) = 0;

  virtual void ReleaseResourceDescriptor(uint32_t index) = 0;
  virtual void ReleaseSamplerDescriptor(uint32_t index) = 0;

  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;
  virtual uint64_t GetFrameID() const = 0;
};