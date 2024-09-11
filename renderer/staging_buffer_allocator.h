#pragma once

#include "../RHI/rhi_context.h"
#include <memory>

class Renderer;

struct StagingBuffer {
  RHIBuffer *buffer;
  uint32_t size;
  uint32_t offset;
};

class StagingBufferAllocator {
public:
  StagingBufferAllocator(Renderer *pRenderer);

  StagingBuffer Allocate(uint32_t size);
  void Reset();

private:
  void CreateNewBuffer();

private:
  Renderer *m_pRenderer = nullptr;
  std::vector<std::unique_ptr<RHIBuffer>> m_pBuffers;
  uint32_t m_nCurrentBuffer = 0;
  uint32_t m_nAllocatedSize = 0;
  uint64_t m_nLastAllocatedFrame = 0;
};