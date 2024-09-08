#pragma once

#include "rhi_resource.h"

class RHIFence;
class RHIBuffer;
class RHITexture;
class RHIPipelineState;

class RHICommandList : public RHIResource
{
public:
	virtual ~RHICommandList() {}

	virtual void Begin() = 0;
	virtual void End() = 0;

	virtual void BeginEvent(const std::string& event_name) = 0;
	virtual void EndEvent() = 0;

	virtual void Wait(RHIFence* fence, uint64_t value) = 0;
	virtual void Signal(RHIFence* fence, uint64_t value) = 0;
	virtual void Submit() = 0;

	//virtual void CopyBuffer(RHIBuffer* dst, uint32_t dst_offset, RHIBuffer* src, uint32_t src_offset, uint32_t size) = 0;
	//virtual void CopyBufferToTexture() = 0;
	//virtual void CopyTexture() = 0;

	virtual void ResourceBarrier(RHIResource* resource, uint32_t sub_resource, RHIResourceState old_state, RHIResourceState new_state) = 0;
	virtual void UavBarrier(RHIResource* resource) = 0;

	virtual void BeginRenderPass(const RHIRenderPassDesc& render_pass) = 0;
	virtual void EndRenderPass() = 0;

	virtual void SetPipelineState(RHIPipelineState* state) = 0;
	virtual void SetIndexBuffer(RHIBuffer* buffer) = 0;
	virtual void SetVertexBuffer(RHIBuffer* buffer, uint8_t slot) = 0;
	virtual void SetConstantBuffer(RHIPipelineType type, uint32_t slot, void* data, size_t data_size) = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void SetScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

	virtual void Draw(uint32_t vertex_count, uint32_t instance_count) = 0;
	virtual void DrawIndexed(uint32_t index_count, uint32_t instance_count) = 0;
};