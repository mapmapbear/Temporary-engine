#pragma once

#include "../rhi_command_list.h"
#include "rhi_d3d12_header.h"

class RHIDevice;
class RHIFence;

class D3D12CommandList : public RHICommandList {
public:
  D3D12CommandList(RHIDevice *pDevice, RHICommandQueue queue_type,
                   const std::string &name);
  ~D3D12CommandList();

  bool Create();

  virtual void *GetHandle() const override { return m_pCommandList; }

  virtual void Begin() override;
  virtual void End() override;

  virtual void BeginEvent(const std::string &event_name) override;
  virtual void EndEvent() override;

  virtual void CopyBufferToTexture(RHITexture *texture, RHIBuffer *buffer,
                                   uint32_t data_offset,
                                   uint32_t data_size) override;

  virtual void Wait(RHIFence *fence, uint64_t value) override;
  virtual void Signal(RHIFence *fence, uint64_t value) override;
  virtual void Submit() override;

  virtual void ResourceBarrier(RHIResource *resource, uint32_t sub_resource,
                               RHIResourceState old_state,
                               RHIResourceState new_state) override;
  virtual void UavBarrier(RHIResource *resource) override;

  virtual void BeginRenderPass(const RHIRenderPassDesc &render_pass) override;
  virtual void EndRenderPass() override;

  virtual void SetPipelineState(RHIPipelineState *state) override;
  virtual void SetIndexBuffer(RHIBuffer *buffer) override;
  virtual void SetVertexBuffer(RHIBuffer *buffer, uint8_t slot) override;
  virtual void SetConstantBuffer(RHIPipelineType type, uint32_t slot,
                                 void *data, size_t data_size) override;
  virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                           uint32_t height) override;
  virtual void SetScissorRect(uint32_t x, uint32_t y, uint32_t width,
                              uint32_t height) override;

  virtual void Draw(uint32_t vertex_count, uint32_t instance_count) override;
  virtual void DrawIndexed(uint32_t index_count, uint32_t instance_count,
                           uint32_t index_offset = 0) override;

private:
  void FlushPendingBarrier();

private:
  RHICommandQueue m_queueType;
  ID3D12CommandQueue *m_pCommandQueue = nullptr;
  ID3D12CommandAllocator *m_pCommandAllocator = nullptr;
  ID3D12GraphicsCommandList4 *m_pCommandList = nullptr;

  RHIPipelineState *m_pCurrentPSO = nullptr;

  std::vector<D3D12_RESOURCE_BARRIER> m_pendingBarriers;
};