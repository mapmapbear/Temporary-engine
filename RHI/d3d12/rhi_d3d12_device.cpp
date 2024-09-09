#include "rhi_d3d12_device.h"
#include "../utils/log.h"
#include "./utils/fassert.h"
#include "D3D12MemAlloc.h"
#include "pix_runtime.h"
#include "rhi_d3d12_buffer.h"
#include "rhi_d3d12_command_list.h"
#include "rhi_d3d12_descriptor.h"
#include "rhi_d3d12_fence.h"
#include "rhi_d3d12_pipeline_state.h"
#include "rhi_d3d12_shader.h"
#include "rhi_d3d12_swapchain.h"
#include "rhi_d3d12_texture.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

extern "C" {
_declspec(dllexport) extern const UINT D3D12SDKVersion = 715;
}
extern "C" {
_declspec(dllexport) extern const char *D3D12SDKPath = u8".\\D3D12\\";
}

static IDXGIAdapter1 *FindAdapter(IDXGIFactory4 *pDXGIFactory,
                                  D3D_FEATURE_LEVEL minimumFeatureLevel) {
  IDXGIAdapter1 *pDXGIAdapter = NULL;

  for (UINT adapterIndex = 0;
       DXGI_ERROR_NOT_FOUND !=
       pDXGIFactory->EnumAdapters1(adapterIndex, &pDXGIAdapter);
       ++adapterIndex) {
    DXGI_ADAPTER_DESC1 desc;
    pDXGIAdapter->GetDesc1(&desc);

    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
      continue;
    }

    if (SUCCEEDED(D3D12CreateDevice(pDXGIAdapter, minimumFeatureLevel,
                                    _uuidof(ID3D12Device), nullptr))) {
      break;
    }

    pDXGIAdapter->Release();
  }

  return pDXGIAdapter;
}

D3D12Device::D3D12Device(const RHIDeviceDesc &desc) { m_desc = desc; }

D3D12Device::~D3D12Device() {
  for (uint32_t i = 0; i < CB_ALLOCATOR_COUNT; ++i) {
    m_pConstantBufferAllocators[i].reset();
  }

  FlushDeferredDeletions();

  m_pRTVAllocator.reset();
  m_pDSVAllocator.reset();
  m_pResDescriptorAllocator.reset();
  m_pSamplerAllocator.reset();

  SAFE_RELEASE(m_pRootSignature);
  SAFE_RELEASE(m_pResourceAllocator);
  SAFE_RELEASE(m_pGraphicsQueue);
  SAFE_RELEASE(m_pCopyQueue);
  SAFE_RELEASE(m_pDxgiAdapter);
  SAFE_RELEASE(m_pDxgiFactory);

#if defined(_DEBUG)
  ID3D12DebugDevice *pDebugDevice = NULL;
  m_pDevice->QueryInterface(IID_PPV_ARGS(&pDebugDevice));
  if (pDebugDevice) {
    pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_SUMMARY |
                                          D3D12_RLDO_DETAIL);
    pDebugDevice->Release();
  }
#endif

  SAFE_RELEASE(m_pDevice);
}

RHIBuffer *D3D12Device::CreateBuffer(const RHIBufferDesc &desc,
                                     const std::string &name) {
  D3D12Buffer *pBuffer = new D3D12Buffer(this, desc, name);
  if (!pBuffer->Create()) {
    delete pBuffer;
    return nullptr;
  }
  return pBuffer;
}

RHITexture *D3D12Device::CreateTexture(const RHITextureDesc &desc,
                                       const std::string &name) {
  D3D12Texture *pTexture = new D3D12Texture(this, desc, name);
  if (!pTexture->Create()) {
    delete pTexture;
    return nullptr;
  }
  return pTexture;
}

RHIFence *D3D12Device::CreateFence(const std::string &name) {
  D3D12Fence *pFence = new D3D12Fence(this, name);
  if (!pFence->Create()) {
    delete pFence;
    return nullptr;
  }
  return pFence;
}

RHISwapchain *D3D12Device::CreateSwapchain(const RHISwapchainDesc &desc,
                                           const std::string &name) {
  D3D12Swapchain *pSwapchain = new D3D12Swapchain(this, desc, name);
  if (!pSwapchain->Create()) {
    delete pSwapchain;
    return nullptr;
  }
  return pSwapchain;
}

RHICommandList *D3D12Device::CreateCommandList(RHICommandQueue queue_type,
                                               const std::string &name) {
  D3D12CommandList *pCommandList = new D3D12CommandList(this, queue_type, name);
  if (!pCommandList->Create()) {
    delete pCommandList;
    return nullptr;
  }
  return pCommandList;
}

RHIShader *D3D12Device::CreateShader(const RHIShaderDesc &desc,
                                     const std::vector<uint8_t> &data,
                                     const std::string &name) {
  return new D3D12Shader(this, desc, data, name);
}

RHIPipelineState *
D3D12Device::CreateGraphicsPipelineState(const RHIGraphicsPipelineDesc &desc,
                                         const std::string &name) {
  D3D12GraphicsPipelineState *pPipeline =
      new D3D12GraphicsPipelineState(this, desc, name);
  if (!pPipeline->Create()) {
    delete pPipeline;
    return nullptr;
  }
  return pPipeline;
}

RHIPipelineState *D3D12Device::CreateMeshShadingPipelineState(
    const RHIMeshShadingPipelineDesc &desc, const std::string &name) {
  // todo
  return nullptr;
}

RHIDescriptor *
D3D12Device::CreateShaderResourceView(RHIResource *resource,
                                      const RHIShaderResourceViewDesc &desc,
                                      const std::string &name) {
  D3D12ShaderResourceView *pSRV =
      new D3D12ShaderResourceView(this, resource, desc, name);
  if (!pSRV->Create()) {
    delete pSRV;
    return nullptr;
  }
  return pSRV;
}

RHIDescriptor *
D3D12Device::CreateUnorderedAccessView(RHIResource *resource,
                                       const RHIUnorderedAccessViewDesc &desc,
                                       const std::string &name) {
  D3D12UnorderedAccessView *pUAV =
      new D3D12UnorderedAccessView(this, resource, desc, name);
  if (!pUAV->Create()) {
    delete pUAV;
    return nullptr;
  }
  return pUAV;
}

RHIDescriptor *
D3D12Device::CreateConstantBufferView(RHIBuffer *buffer,
                                      const RHIConstantBufferViewDesc &desc,
                                      const std::string &name) {
  D3D12ConstantBufferView *pCBV =
      new D3D12ConstantBufferView(this, buffer, desc, name);
  if (!pCBV->Create()) {
    delete pCBV;
    return nullptr;
  }
  return pCBV;
}

RHIDescriptor *D3D12Device::CreateSampler(const RHISamplerDesc &desc,
                                          const std::string &name) {
  D3D12Sampler *pSampler = new D3D12Sampler(this, desc, name);
  if (!pSampler->Create()) {
    delete pSampler;
    return nullptr;
  }
  return pSampler;
}

void D3D12Device::ReleaseResourceDescriptor(uint32_t index) {
  D3D12Descriptor descriptor = m_pResDescriptorAllocator->GetDescriptor(index);
  m_resourceDeletionQueue.push({descriptor, m_nFrameID});
}

void D3D12Device::ReleaseSamplerDescriptor(uint32_t index) {
  D3D12Descriptor descriptor = m_pSamplerAllocator->GetDescriptor(index);
  m_samplerDeletionQueue.push({descriptor, m_nFrameID});
}

void D3D12Device::FlushDeferredDeletions() { DoDeferredDeletion(true); }

void D3D12Device::BeginFrame() {
  DoDeferredDeletion();

  uint32_t index = m_nFrameID % CB_ALLOCATOR_COUNT;
  m_pConstantBufferAllocators[index]->Reset();
}

void D3D12Device::EndFrame() { ++m_nFrameID; }

bool D3D12Device::Init() {
  UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
  ID3D12Debug1 *debugController = nullptr;

  if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
    debugController->EnableDebugLayer();
    // debugController1->SetEnableGPUBasedValidation(TRUE);

    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
  }

  SAFE_RELEASE(debugController);
#endif

  if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags,
                                IID_PPV_ARGS(&m_pDxgiFactory)))) {
    return false;
  }

  D3D_FEATURE_LEVEL minimumFeatureLevel = D3D_FEATURE_LEVEL_12_0;
  m_pDxgiAdapter = FindAdapter(m_pDxgiFactory, minimumFeatureLevel);
  if (m_pDxgiAdapter == nullptr) {
    return false;
  }

  if (FAILED(D3D12CreateDevice(m_pDxgiAdapter, minimumFeatureLevel,
                               IID_PPV_ARGS(&m_pDevice)))) {
    return false;
  }

  D3D12_FEATURE_DATA_SHADER_MODEL shaderModel;
  shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_7;
  if (FAILED(m_pDevice->CheckFeatureSupport(
          D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))) {
    return false;
  }

  // SM6.6 is required !
  RE_ASSERT(shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_6);

  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pGraphicsQueue));
  m_pGraphicsQueue->SetName(L"Graphics Queue");

  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
  m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCopyQueue));
  m_pCopyQueue->SetName(L"Copy Queue");

  D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
  allocatorDesc.pDevice = m_pDevice;
  allocatorDesc.pAdapter = m_pDxgiAdapter;
  if (FAILED(D3D12MA::CreateAllocator(&allocatorDesc, &m_pResourceAllocator))) {
    return false;
  }

  for (uint32_t i = 0; i < CB_ALLOCATOR_COUNT; ++i) {
    std::string name = "CB Allocator " + std::to_string(i);
    m_pConstantBufferAllocators[i] =
        std::make_unique<D3D12ConstantBufferAllocator>(this, 4 * 1024 * 1024,
                                                       name);
  }

  m_pRTVAllocator = std::make_unique<D3D12DescriptorAllocator>(
      m_pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 512, "RTV Heap");
  m_pDSVAllocator = std::make_unique<D3D12DescriptorAllocator>(
      m_pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 128, "DSV Heap");
  m_pResDescriptorAllocator = std::make_unique<D3D12DescriptorAllocator>(
      m_pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 65536,
      "Resource Heap");
  m_pSamplerAllocator = std::make_unique<D3D12DescriptorAllocator>(
      m_pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 32, "Sampler Heap");

  CreateRootSignature();

  pix::Init();

  return true;
}

D3D12_GPU_VIRTUAL_ADDRESS
D3D12Device::AllocateConstantBuffer(void *data, size_t data_size) {
  void *cpu_address;
  uint64_t gpu_address;

  uint32_t index = m_nFrameID % CB_ALLOCATOR_COUNT;
  m_pConstantBufferAllocators[index]->Allocate((uint32_t)data_size,
                                               &cpu_address, &gpu_address);

  memcpy(cpu_address, data, data_size);

  return gpu_address;
}

void D3D12Device::Delete(IUnknown *object) {
  m_deletionQueue.push({object, m_nFrameID});
}

void D3D12Device::Delete(D3D12MA::Allocation *allocation) {
  m_allocationDeletionQueue.push({allocation, m_nFrameID});
}

D3D12Descriptor D3D12Device::AllocateRTV() {
  return m_pRTVAllocator->Allocate();
}

D3D12Descriptor D3D12Device::AllocateDSV() {
  return m_pDSVAllocator->Allocate();
}

void D3D12Device::DeleteRTV(const D3D12Descriptor &descriptor) {
  if (!IsNullDescriptor(descriptor)) {
    m_rtvDeletionQueue.push({descriptor, m_nFrameID});
  }
}

void D3D12Device::DeleteDSV(const D3D12Descriptor &descriptor) {
  if (!IsNullDescriptor(descriptor)) {
    m_dsvDeletionQueue.push({descriptor, m_nFrameID});
  }
}

D3D12Descriptor D3D12Device::AllocateResourceDescriptor() {
  return m_pResDescriptorAllocator->Allocate();
}

D3D12Descriptor D3D12Device::AllocateSampler() {
  return m_pSamplerAllocator->Allocate();
}

void D3D12Device::DeleteResourceDescriptor(const D3D12Descriptor &descriptor) {
  if (!IsNullDescriptor(descriptor)) {
    m_resourceDeletionQueue.push({descriptor, m_nFrameID});
  }
}

void D3D12Device::DeleteSampler(const D3D12Descriptor &descriptor) {
  if (!IsNullDescriptor(descriptor)) {
    m_samplerDeletionQueue.push({descriptor, m_nFrameID});
  }
}

void D3D12Device::DoDeferredDeletion(bool force_delete) {
  while (!m_deletionQueue.empty()) {
    auto item = m_deletionQueue.front();
    if (!force_delete && item.frame + m_desc.max_frame_lag > m_nFrameID) {
      break;
    }

    SAFE_RELEASE(item.object);
    m_deletionQueue.pop();
  }

  while (!m_allocationDeletionQueue.empty()) {
    auto item = m_allocationDeletionQueue.front();
    if (!force_delete && item.frame + m_desc.max_frame_lag > m_nFrameID) {
      break;
    }

    SAFE_RELEASE(item.allocation);
    m_allocationDeletionQueue.pop();
  }

  while (!m_rtvDeletionQueue.empty()) {
    auto item = m_rtvDeletionQueue.front();
    if (!force_delete && item.frame + m_desc.max_frame_lag > m_nFrameID) {
      break;
    }

    m_pRTVAllocator->Free(item.descriptor);
    m_rtvDeletionQueue.pop();
  }

  while (!m_dsvDeletionQueue.empty()) {
    auto item = m_dsvDeletionQueue.front();
    if (!force_delete && item.frame + m_desc.max_frame_lag > m_nFrameID) {
      break;
    }

    m_pDSVAllocator->Free(item.descriptor);
    m_dsvDeletionQueue.pop();
  }

  while (!m_resourceDeletionQueue.empty()) {
    auto item = m_resourceDeletionQueue.front();
    if (!force_delete && item.frame + m_desc.max_frame_lag > m_nFrameID) {
      break;
    }

    m_pResDescriptorAllocator->Free(item.descriptor);
    m_resourceDeletionQueue.pop();
  }

  while (!m_samplerDeletionQueue.empty()) {
    auto item = m_samplerDeletionQueue.front();
    if (!force_delete && item.frame + m_desc.max_frame_lag > m_nFrameID) {
      break;
    }

    m_pSamplerAllocator->Free(item.descriptor);
    m_samplerDeletionQueue.pop();
  }
}

void D3D12Device::CreateRootSignature() {
  // AMD : Try to stay below 13 DWORDs https://gpuopen.com/performance/
  // 4 root constants + 4 root CBVs == 12 DWORDs, everything else is bindless

  CD3DX12_ROOT_PARAMETER1 root_parameters[5] = {};
  root_parameters[0].InitAsConstants(4, 0);
  root_parameters[1].InitAsConstantBufferView(1);
  root_parameters[2].InitAsConstantBufferView(2);
  root_parameters[3].InitAsConstantBufferView(3);
  root_parameters[4].InitAsConstantBufferView(4);

  D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
      D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
      D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
  rootSignatureDesc.Init_1_1(_countof(root_parameters), root_parameters, 0,
                             nullptr, rootSignatureFlags);

  D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
  featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

  ID3DBlob *signature = nullptr;
  ID3DBlob *error = nullptr;
  HRESULT hr = D3DX12SerializeVersionedRootSignature(
      &rootSignatureDesc, featureData.HighestVersion, &signature, &error);
  RE_ASSERT(SUCCEEDED(hr));

  hr = m_pDevice->CreateRootSignature(0, signature->GetBufferPointer(),
                                      signature->GetBufferSize(),
                                      IID_PPV_ARGS(&m_pRootSignature));
  RE_ASSERT(SUCCEEDED(hr));

  SAFE_RELEASE(signature);
  SAFE_RELEASE(error);

  m_pRootSignature->SetName(L"D3D12Device::m_pRootSignature");
}

D3D12DescriptorAllocator::D3D12DescriptorAllocator(
    ID3D12Device *device, D3D12_DESCRIPTOR_HEAP_TYPE type,
    uint32_t descriptor_count, const std::string &name) {
  m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);
  m_descirptorCount = descriptor_count;
  m_allocatedCount = 0;
  m_bShaderVisible = (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ||
                      type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

  D3D12_DESCRIPTOR_HEAP_DESC desc = {};
  desc.Type = type;
  desc.NumDescriptors = descriptor_count;
  if (m_bShaderVisible) {
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  }
  device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeap));

  m_pHeap->SetName(string_to_wstring(name).c_str());
}

D3D12DescriptorAllocator::~D3D12DescriptorAllocator() { m_pHeap->Release(); }

D3D12Descriptor D3D12DescriptorAllocator::Allocate() {
  if (!m_freeDescriptors.empty()) {
    D3D12Descriptor descriptor = m_freeDescriptors.back();
    m_freeDescriptors.pop_back();
    return descriptor;
  }

  RE_ASSERT(m_allocatedCount <= m_descirptorCount);

  D3D12Descriptor descriptor = GetDescriptor(m_allocatedCount);

  m_allocatedCount++;

  return descriptor;
}

void D3D12DescriptorAllocator::Free(const D3D12Descriptor &descriptor) {
  m_freeDescriptors.push_back(descriptor);
}

D3D12Descriptor D3D12DescriptorAllocator::GetDescriptor(uint32_t index) const {
  D3D12Descriptor descriptor;
  descriptor.cpu_handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
      m_pHeap->GetCPUDescriptorHandleForHeapStart(), index, m_descriptorSize);

  if (m_bShaderVisible) {
    descriptor.gpu_handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
        m_pHeap->GetGPUDescriptorHandleForHeapStart(), index, m_descriptorSize);
  }

  descriptor.index = index;

  return descriptor;
}

D3D12ConstantBufferAllocator::D3D12ConstantBufferAllocator(
    D3D12Device *device, uint32_t buffer_size, const std::string &name) {
  RHIBufferDesc desc;
  desc.size = buffer_size;
  desc.memory_type = RHIMemoryType::CpuToGpu;
  desc.usage = GfxBufferUsageConstantBuffer;

  m_pBuffer.reset(device->CreateBuffer(desc, name));
}

#define ALIGN(bytes, alignment)                                                \
  (((bytes) + (alignment) - 1) & ~((alignment) - 1))

void D3D12ConstantBufferAllocator::Allocate(uint32_t size, void **cpu_address,
                                            uint64_t *gpu_address) {
  RE_ASSERT(m_allcatedSize + size <= m_pBuffer->GetDesc().size);

  *cpu_address = (char *)m_pBuffer->GetCpuAddress() + m_allcatedSize;
  *gpu_address = m_pBuffer->GetGpuAddress() + m_allcatedSize;

  m_allcatedSize += ALIGN(size, 256); // alignment be a multiple of 256
}

void D3D12ConstantBufferAllocator::Reset() { m_allcatedSize = 0; }
