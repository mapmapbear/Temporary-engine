#pragma once

#include "renderer/renderer.h"
#include <cstdint>
#include <memory>

class GUI {
public:
  GUI();
  ~GUI();

  bool Init();
  void NewFrame();
  void Render(RHICommandList *pCommandList);

private:
  void SetupRenderStates(RHICommandList *pCommandList, uint32_t frame_index);

private:
  RHIPipelineState *m_pPSO = nullptr;
  std::unique_ptr<RHITexture> m_pFontTexture;
  std::unique_ptr<RHIDescriptor> m_pFontTextureSRV;
  std::unique_ptr<RHIBuffer> m_pVertexBuffer[MAX_INFLIGHT_FRAMES];
  std::unique_ptr<RHIBuffer> m_pIndexBuffer[MAX_INFLIGHT_FRAMES];
  std::unique_ptr<RHIDescriptor> m_pVertexBufferSRV[MAX_INFLIGHT_FRAMES];
};