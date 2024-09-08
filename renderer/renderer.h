#pragma once

#include "../utils/fmath.h"
#include "RHI/rhi_context.h"
#include "pipeline_cache.h"
#include "shader_cache.h"
#include "shader_compiler.h"
#include <memory>

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

  RHIShader *GetShader(const std::string &file, const std::string &entry_point,
                       const std::string &profile,
                       const std::vector<std::string> &defines);
  RHIPipelineState *GetPipelineState(const RHIGraphicsPipelineDesc &desc,
                                     const std::string &name);
  void OnWindowResize(uint32_t width, uint32_t height);

private:
  void CreateCommonResources();

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

  std::unique_ptr<ShaderCompiler> m_pShaderCompiler;
  std::unique_ptr<ShaderCache> m_pShaderCache;
  std::unique_ptr<PipelineStateCache> m_pPipelineCache;

  uint32_t m_nPointSampler = 0;
  uint32_t m_nLinearSampler = 0;
};