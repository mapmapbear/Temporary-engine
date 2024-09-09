#include "renderer.h"
#include "core/engine.h"
#include "renderer/renderer.h"

Renderer::Renderer() {
  m_pShaderCompiler = std::make_unique<ShaderCompiler>();
  m_pShaderCache = std::make_unique<ShaderCache>(this);
  m_pPipelineCache = std::make_unique<PipelineStateCache>(this);

  vList = {
      // front face
      {-0.5f, 0.5f, -0.5f, 0.f, 0.f},
      {0.5f, -0.5f, -0.5f, 1.f, 1.f},
      {-0.5f, -0.5f, -0.5f, 0.f, 1.f},
      {0.5f, 0.5f, -0.5f, 1.f, 0.f},

      // right side face
      {0.5f, -0.5f, -0.5f, 0.f, 1.f},
      {0.5f, 0.5f, 0.5f, 1.f, 0.f},
      {0.5f, -0.5f, 0.5f, 1.f, 1.f},
      {0.5f, 0.5f, -0.5f, 0.f, 0.f},

      // left side face
      {-0.5f, 0.5f, 0.5f, 0.f, 0.f},
      {-0.5f, -0.5f, -0.5f, 1.f, 1.f},
      {-0.5f, -0.5f, 0.5f, 0.f, 1.f},
      {-0.5f, 0.5f, -0.5f, 1.f, 0.f},

      // back face
      {0.5f, 0.5f, 0.5f, 0.f, 0.f},
      {-0.5f, -0.5f, 0.5f, 1.f, 1.f},
      {0.5f, -0.5f, 0.5f, 0.f, 1.f},
      {-0.5f, 0.5f, 0.5f, 1.f, 0.f},

      // top face
      {-0.5f, 0.5f, -0.5f, 0.f, 0.f},
      {0.5f, 0.5f, 0.5f, 1.f, 1.f},
      {0.5f, 0.5f, -0.5f, 0.f, 1.f},
      {-0.5f, 0.5f, 0.5f, 1.f, 0.f},

      // bottom face
      {0.5f, -0.5f, 0.5f, 0.f, 0.f},
      {-0.5f, -0.5f, -0.5f, 1.f, 1.f},
      {0.5f, -0.5f, -0.5f, 0.f, 1.f},
      {-0.5f, -0.5f, 0.5f, 1.f, 0.f},
  };
  vBufferSize = sizeof(vList);
}

Renderer::~Renderer() {
  WaitGpuFinished();
  m_pFrameFence->Wait(m_nCurrentFenceValue);
}

void Renderer::CreateDevice(void *window_handle, uint32_t window_width,
                            uint32_t window_height, bool enable_vsync) {
  RHIDeviceDesc desc;
  desc.max_frame_lag = MAX_INFLIGHT_FRAMES;
  m_pDevice.reset(CreateGfxDevice(desc));

  RHISwapchainDesc swapchainDesc;
  swapchainDesc.window_handle = window_handle;
  swapchainDesc.width = window_width;
  swapchainDesc.height = window_height;
  swapchainDesc.enable_vsync = enable_vsync;
  m_pSwapchain.reset(
      m_pDevice->CreateSwapchain(swapchainDesc, "Renderer::m_pSwapchain"));

  m_pFrameFence.reset(m_pDevice->CreateFence("Renderer::m_pFrameFence"));

  for (int i = 0; i < MAX_INFLIGHT_FRAMES; ++i) {
    std::string name = "Renderer::m_pCommandLists[" + std::to_string(i) + "]";
    m_pCommandLists[i].reset(
        m_pDevice->CreateCommandList(RHICommandQueue::Graphics, name));
  }
  m_pUploadFence.reset(m_pDevice->CreateFence("Renderer::m_pUploadFence"));
  for (int i = 0; i < MAX_INFLIGHT_FRAMES; ++i) {
    std::string name =
        "Renderer::m_pUploadCommandList[" + std::to_string(i) + "]";
    m_pUploadCommandList[i].reset(
        m_pDevice->CreateCommandList(RHICommandQueue::Copy, name));

    m_pStagingBufferAllocator[i] =
        std::make_unique<StagingBufferAllocator>(this);
  }

  CreateCommonResources();
}

void Renderer::RenderFrame() {
  BeginFrame();
  UploadResources();
  Render();
  EndFrame();
}

void Renderer::BeginFrame() {
  uint32_t frame_index = m_pDevice->GetFrameID() % MAX_INFLIGHT_FRAMES;
  m_pFrameFence->Wait(m_nFrameFenceValue[frame_index]);
  m_pDevice->BeginFrame();

  RHICommandList *pCommandList = m_pCommandLists[frame_index].get();
  pCommandList->Begin();
}

void Renderer::UploadResources() {
  if (m_pendingTextureUploads.empty() && m_pendingBufferUpload.empty()) {
    return;
  }
  uint32_t frame_index = m_pDevice->GetFrameID() % MAX_INFLIGHT_FRAMES;
  RHICommandList *pUploadCommandList = m_pUploadCommandList[frame_index].get();
  pUploadCommandList->Begin();

  {
    RENDER_EVENT(pUploadCommandList, "Renderer::UploadResources");

    for (size_t i = 0; i < m_pendingTextureUploads.size(); ++i) {
      const TextureUpload &upload = m_pendingTextureUploads[i];
      pUploadCommandList->CopyBufferToTexture(
          upload.texture, upload.staging_buffer.buffer,
          upload.staging_buffer.offset, upload.staging_buffer.size);
    }
    m_pendingTextureUploads.clear();

    for (size_t i = 0; i < m_pendingBufferUpload.size(); ++i) {
    }
    m_pendingBufferUpload.clear();
  }

  pUploadCommandList->End();
  pUploadCommandList->Submit();

  m_nCurrentUploadFenceValue++;
  pUploadCommandList->Signal(m_pUploadFence.get(), m_nCurrentUploadFenceValue);

  RHICommandList *pCommandList = m_pCommandLists[frame_index].get();
  pCommandList->Wait(m_pUploadFence.get(), m_nCurrentUploadFenceValue);
}

void Renderer::Render() {
  uint32_t frame_index = m_pDevice->GetFrameID() % MAX_INFLIGHT_FRAMES;
  RHICommandList *pCommandList = m_pCommandLists[frame_index].get();

  pCommandList->BeginEvent("Renderer::RenderFrame");

  RHITexture *pBackBuffer = m_pSwapchain->GetBackBuffer();
  pCommandList->ResourceBarrier(pBackBuffer, 0, RHIResourceState::Present,
                                RHIResourceState::RenderTarget);

  RHIRenderPassDesc render_pass;
  render_pass.color[0].texture = pBackBuffer;
  render_pass.color[0].load_op = RHIRenderPassLoadOp::Clear;
  render_pass.color[0].clear_color = glm::vec4(0.0f);
  pCommandList->BeginRenderPass(render_pass);

  pCommandList->SetViewport(0, 0, pBackBuffer->GetDesc().width,
                            pBackBuffer->GetDesc().height);

  {
    RENDER_EVENT(pCommandList, "Render something");

    RHIGraphicsPipelineDesc psoDesc;
    psoDesc.vs = GetShader("mesh.hlsl", "vs_main", "vs_6_6", {});
    psoDesc.ps = GetShader("mesh.hlsl", "ps_main", "ps_6_6", {});
    psoDesc.rt_format[0] = m_pSwapchain->GetBackBuffer()->GetDesc().format;

    RHIPipelineState *pPSO = GetPipelineState(psoDesc, "test pso");
    pCommandList->SetPipelineState(pPSO);
    Camera *camera = Engine::GetInstance()->GetWorld()->GetCamera();
    pCommandList->SetConstantBuffer(RHIPipelineType::Graphics, 1,
                                    (void *)&camera->GetViewProjectionMatrix(),
                                    sizeof(glm::mat4));

    pCommandList->Draw(3, 1);
  }

  GUI *pGui = Engine::GetInstance()->GetWorld()->GetGUI();
  pGui->Render(pCommandList);

  pCommandList->EndRenderPass();
  pCommandList->ResourceBarrier(pBackBuffer, 0, RHIResourceState::RenderTarget,
                                RHIResourceState::Present);
}

void Renderer::EndFrame() {
  uint32_t frame_index = m_pDevice->GetFrameID() % MAX_INFLIGHT_FRAMES;
  RHICommandList *pCommandList = m_pCommandLists[frame_index].get();
  pCommandList->End();

  ++m_nCurrentFenceValue;
  m_nFrameFenceValue[frame_index] = m_nCurrentFenceValue;

  pCommandList->Submit();
  m_pSwapchain->Present();
  pCommandList->Signal(m_pFrameFence.get(), m_nCurrentFenceValue);

  m_pStagingBufferAllocator[frame_index]->Reset();

  m_pDevice->EndFrame();
}

RHIShader *Renderer::GetShader(const std::string &file,
                               const std::string &entry_point,
                               const std::string &profile,
                               const std::vector<std::string> &defines) {
  return m_pShaderCache->GetShader(file, entry_point, profile, defines);
}

RHIPipelineState *
Renderer::GetPipelineState(const RHIGraphicsPipelineDesc &desc,
                           const std::string &name) {
  return m_pPipelineCache->GetPipelineState(desc, name);
}

void Renderer::CreateCommonResources() {
  RHISamplerDesc desc;
  m_pPointSampler.reset(
      m_pDevice->CreateSampler(desc, "Renderer::m_pPointSampler"));

  desc.min_filter = RHIFilter::Linear;
  desc.mag_filter = RHIFilter::Linear;
  desc.mip_filter = RHIFilter::Linear;
  m_pLinearSampler.reset(
      m_pDevice->CreateSampler(desc, "Renderer::m_pLinearSampler"));
}

void Renderer::WaitGpuFinished() { m_pFrameFence->Wait(m_nCurrentFenceValue); }

void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
  WaitGpuFinished();

  m_pSwapchain->Resize(width, height);
}

void Renderer::UploadTexture(RHITexture *texture, void *data,
                             uint32_t data_size) {
    uint32_t frame_index = m_pDevice->GetFrameID() % MAX_INFLIGHT_FRAMES;
    StagingBufferAllocator* pAllocator = m_pStagingBufferAllocator[frame_index].get();

    StagingBuffer buffer = pAllocator->Allocate(data_size);
    memcpy((char*)buffer.buffer->GetCpuAddress() + buffer.offset, data, data_size);

    m_pendingTextureUploads.push_back({ texture, buffer });
}

void Renderer::UploadBuffer(RHIBuffer *buffer, void *data, uint32_t data_size) {
}
