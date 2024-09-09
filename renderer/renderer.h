#pragma once

#include "../utils/fmath.h"
#include "RHI/rhi_context.h"
#include "RHI/rhi_fence.h"
#include "pipeline_cache.h"
#include "shader_cache.h"
#include "shader_compiler.h"
#include "staging_buffer_allocator.h"
#include <memory>

const static int MAX_INFLIGHT_FRAMES = 3;

struct Vertex {
  float3 pos;
  float2 texCoord;

  Vertex() {}
  Vertex(float x, float y, float z, float tx, float ty)
      : pos(x, y, z), texCoord(tx, ty) {}
};

class Renderer {
public:
  Renderer();
  ~Renderer();

  void CreateDevice(void *window_handle, uint32_t window_width,
                    uint32_t window_height, bool enable_vsync);
  void RenderFrame();
  void WaitGpuFinished();
  RHIDevice *GetDevice() const { return m_pDevice.get(); }
  ShaderCompiler *GetShaderCompiler() const { return m_pShaderCompiler.get(); }
  RHISwapchain *GetSwapchain() const { return m_pSwapchain.get(); }
  RHIShader *GetShader(const std::string &file, const std::string &entry_point,
                       const std::string &profile,
                       const std::vector<std::string> &defines);
  RHIPipelineState *GetPipelineState(const RHIGraphicsPipelineDesc &desc,
                                     const std::string &name);
  RHIDescriptor *GetPointSampler() const { return m_pPointSampler.get(); }
  RHIDescriptor *GetLinearSampler() const { return m_pLinearSampler.get(); }

  void UploadTexture(RHITexture *texture, void *data, uint32_t data_size);
  void UploadBuffer(RHIBuffer *buffer, void *data, uint32_t data_size);

  void OnWindowResize(uint32_t width, uint32_t height);

private:
  void CreateCommonResources();
  void BeginFrame();
  void UploadResources();
  void Render();
  void EndFrame();

private:
  std::vector<Vertex> vList;
  uint32_t vBufferSize;

private:
  std::unique_ptr<RHIDevice> m_pDevice;
  std::unique_ptr<RHISwapchain> m_pSwapchain;

  std::unique_ptr<RHIFence> m_pFrameFence;
  uint64_t m_nCurrentFenceValue = 0;

  const static int MAX_INFLIGHT_FRAMES = 3;
  uint64_t m_nFrameFenceValue[MAX_INFLIGHT_FRAMES] = {};
  std::unique_ptr<RHICommandList> m_pCommandLists[MAX_INFLIGHT_FRAMES];

  std::unique_ptr<RHIFence> m_pUploadFence;
  uint64_t m_nCurrentUploadFenceValue = 0;
  std::unique_ptr<RHICommandList> m_pUploadCommandList[MAX_INFLIGHT_FRAMES];
  std::unique_ptr<StagingBufferAllocator>
      m_pStagingBufferAllocator[MAX_INFLIGHT_FRAMES];

  struct TextureUpload {
    RHITexture *texture;
    StagingBuffer staging_buffer;
  };
  std::vector<TextureUpload> m_pendingTextureUploads;

  struct BufferUpload {};
  std::vector<BufferUpload> m_pendingBufferUpload;
  std::unique_ptr<ShaderCompiler> m_pShaderCompiler;
  std::unique_ptr<ShaderCache> m_pShaderCache;
  std::unique_ptr<PipelineStateCache> m_pPipelineCache;

  std::unique_ptr<RHIDescriptor> m_pPointSampler;
  std::unique_ptr<RHIDescriptor> m_pLinearSampler;
};