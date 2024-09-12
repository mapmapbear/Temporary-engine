#pragma once

#include "rhi/rhi_context.h"
#include <memory>

class Renderer;

class Texture
{
public:
    Texture(Renderer* pRenderer, const std::string& file);

    bool Create();

    RHITexture* GetTexture() const { return m_pTexture.get(); }
    RHIDescriptor* GetSRV() const { return m_pSRV.get(); }

private:
    Renderer* m_pRenderer;
    std::string m_file;
    std::unique_ptr<RHITexture> m_pTexture;
    std::unique_ptr<RHIDescriptor> m_pSRV;
};