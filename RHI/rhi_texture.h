#pragma once

#include "rhi_resource.h"

class RHITexture : public RHIResource {
public:
  const RHITextureDesc &GetDesc() const { return m_desc; }
  virtual uint32_t GetRequiredStagingBufferSize() const = 0;
  virtual uint32_t GetRowPitch(uint32_t mip_level) const = 0;

protected:
  RHITextureDesc m_desc = {};
};
