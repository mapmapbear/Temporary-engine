#pragma once

#include "rhi/rhi_context.h"
#include <memory>

class RenderTarget
{
public:
    RenderTarget(bool auto_resize, float size, const std::string& name);
    ~RenderTarget();

    bool Create(uint32_t width, uint32_t height, RHIFormat format, RHITextureUsageFlags flags);

    RHITexture* GetTexture() const { return m_pTexture.get(); }
    RHIDescriptor* GetSRV() const { return m_pSRV.get(); }
    RHIDescriptor* GetUAV() const { return m_pUAV.get(); }
    void OnWindowResize(uint32_t width, uint32_t height);

private:
    std::unique_ptr<RHITexture> m_pTexture;
    std::unique_ptr<RHIDescriptor> m_pSRV;
    std::unique_ptr<RHIDescriptor> m_pUAV;

    std::string m_name;
    bool m_bAutoResize;
    float m_size;
};