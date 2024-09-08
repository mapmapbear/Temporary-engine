#pragma once

#include "../rhi_swapchain.h"
#include "rhi_d3d12_header.h"
#include <vector>


class D3D12Device;

class D3D12Swapchain : public RHISwapchain {
public:
  D3D12Swapchain(D3D12Device *pDevice, const RHISwapchainDesc &desc,
                 const std::string &name);
  ~D3D12Swapchain();

  virtual void *GetHandle() const override { return m_pSwapChain; }
  virtual bool Present() override;
  virtual bool Resize(uint32_t width, uint32_t height) override;
  virtual RHITexture *GetBackBuffer() const override;

  bool Create();

private:
  bool CreateTextures();

private:
  IDXGISwapChain3 *m_pSwapChain = nullptr;

  uint32_t m_nCurrentBackBuffer = 0;
  std::vector<RHITexture *> m_backBuffers;
};