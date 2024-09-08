#pragma once

#include "fmath.h"
#include <stdint.h>
#include <string>
#include <vector>

enum class RHIRenderBackend {
  D3D12,
  // todo : maybe Vulkan
};

enum class RHIFormat {
  Unknown,

  RGBA32F,
  RGBA32UI,
  RGBA32SI,
  RGBA16F,
  RGBA16UI,
  RGBA16SI,
  RGBA16UNORM,
  RGBA16SNORM,
  RGBA8UI,
  RGBA8SI,
  RGBA8UNORM,
  RGBA8SNORM,
  RGBA8SRGB,

  RG32F,
  RG32UI,
  RG32SI,
  RG16F,
  RG16UI,
  RG16SI,
  RG16UNORM,
  RG16SNORM,
  RG8UI,
  RG8SI,
  RG8UNORM,
  RG8SNORM,

  R32F,
  R32UI,
  R32SI,
  R16F,
  R16UI,
  R16SI,
  R16UNORM,
  R16SNORM,
  R8UI,
  R8SI,
  R8UNORM,
  R8SNORM,

  D32F,
  D32FS8,
  D16,
};

enum class RHIMemoryType {
  GpuOnly,
  CpuOnly,  // staging buffers
  CpuToGpu, // frequently updated buffers
  GpuToCpu, // readback
};

enum class RHIAllocationType {
  Committed,
  Placed,
  // todo : SubAllocatedBuffer,
};

enum RHIBufferUsageBit {
  GfxBufferUsageConstantBuffer = 1 << 0,
  GfxBufferUsageStructuredBuffer = 1 << 1,
  GfxBufferUsageTypedBuffer = 1 << 2,
  GfxBufferUsageRawBuffer = 1 << 3,
  GfxBufferUsageUnorderedAccess = 1 << 4,
};
using GfxBufferUsageFlags = uint32_t;

enum RHITextureUsageBit {
  GfxTextureUsageShaderResource = 1 << 0,
  GfxTextureUsageRenderTarget = 1 << 1,
  GfxTextureUsageDepthStencil = 1 << 2,
  GfxTextureUsageUnorderedAccess = 1 << 3,
};
using RHITextureUsageFlags = uint32_t;

enum class RHITextureType {
  Texture2D,
  Texture2DArray,
  Texture3D,
  TextureCube,
  TextureCubeArray,
};

enum class RHICommandQueue {
  Graphics,
  Compute,
  Copy,
};

enum class RHIResourceState {
  Common,
  RenderTarget,
  UnorderedAccess,
  DepthStencil,
  DepthStencilReadOnly,
  ShaderResource,
  ShaderResourcePSOnly,
  IndirectArg,
  CopyDst,
  CopySrc,
  ResolveDst,
  ResolveSrc,
  Present,
};

enum class RHIRenderPassLoadOp {
  Load,
  Clear,
  DontCare,
};

enum class RHIRenderPassStoreOp {
  Store,
  DontCare,
};

enum class RHIShaderResourceViewType {
  Texture2D,
  Texture2DArray,
  Texture3D,
  TextureCube,
  TextureCubeArray,
  StructuredBuffer,
  TypedBuffer,
  RawBuffer,
};

enum class RHIUnorderedAccessViewType {
  Texture2D,
  Texture2DArray,
  Texture3D,
  StructuredBuffer,
  TypedBuffer,
  RawBuffer,
};

static const uint32_t GFX_ALL_SUB_RESOURCE = 0xFFFFFFFF;

struct RHIDeviceDesc {
  RHIRenderBackend backend = RHIRenderBackend::D3D12;
  uint32_t max_frame_lag = 3;
};

struct RHISwapchainDesc {
  void *window_handle = nullptr;
  uint32_t width = 1;
  uint32_t height = 1;
  uint32_t backbuffer_count = 3;
  RHIFormat backbuffer_format = RHIFormat::RGBA8UNORM;
  bool enable_vsync = true;
};

struct RHIBufferDesc {
  uint32_t stride = 1;
  uint32_t size = 1;
  RHIFormat format = RHIFormat::Unknown;
  RHIMemoryType memory_type = RHIMemoryType::GpuOnly;
  RHIAllocationType alloc_type = RHIAllocationType::Committed;
  GfxBufferUsageFlags usage = 0;
};

struct RHITextureDesc {
  uint32_t width = 1;
  uint32_t height = 1;
  uint32_t depth = 1;
  uint32_t mip_levels = 1;
  uint32_t array_size = 1;
  RHITextureType type = RHITextureType::Texture2D;
  RHIFormat format = RHIFormat::Unknown;
  RHIMemoryType memory_type = RHIMemoryType::GpuOnly;
  RHIAllocationType alloc_type = RHIAllocationType::Committed;
  RHITextureUsageFlags usage = GfxTextureUsageShaderResource;
};

struct RHIConstantBufferViewDesc {
  uint32_t size = 0;
  uint32_t offset = 0;
};

struct RHIShaderResourceViewDesc {
  RHIShaderResourceViewType type;

  union {
    struct {
      uint32_t mip_slice = 0;
      uint32_t mip_levels = uint32_t(-1);
      uint32_t array_slice = 0;
      uint32_t array_size = 1;
      uint32_t plane_slice = 0;
    } texture;

    struct {
      uint32_t size;
      uint32_t offset;
    } buffer;
  };
};

struct RHIUnorderedAccessViewDesc {
  RHIUnorderedAccessViewType type;

  union {
    struct {
      uint32_t mip_slice = 0;
      uint32_t array_slice = 0;
      uint32_t array_size = 1;
      uint32_t plane_slice = 0;
    } texture;

    struct {
      uint32_t size;
      uint32_t offset;
    } buffer;
  };
};

class RHITexture;

struct RHIRenderPassColorAttachment {
  RHITexture *texture = nullptr;
  uint32_t mip_slice = 0;
  uint32_t array_slice = 0;
  RHIRenderPassLoadOp load_op = RHIRenderPassLoadOp::Load;
  RHIRenderPassStoreOp store_op = RHIRenderPassStoreOp::Store;
  float4 clear_color = {};
};

struct RHIRenderPassDepthAttachment {
  RHITexture *texture = nullptr;
  uint32_t mip_slice = 0;
  uint32_t array_slice = 0;
  RHIRenderPassLoadOp load_op = RHIRenderPassLoadOp::Load;
  RHIRenderPassStoreOp store_op = RHIRenderPassStoreOp::Store;
  RHIRenderPassLoadOp stencil_load_op = RHIRenderPassLoadOp::Load;
  RHIRenderPassStoreOp stencil_store_op = RHIRenderPassStoreOp::Store;
  float clear_depth = 0.0f;
  uint32_t clear_stencil = 0;
};

struct RHIRenderPassDesc {
  RHIRenderPassColorAttachment color[8];
  RHIRenderPassDepthAttachment depth;
};

struct RHIShaderDesc {
  std::string file;
  std::string entry_point;
  std::string profile;
  std::vector<std::string> defines;
};

enum class RHICullMode {
  None,
  Front,
  Back,
};

struct RHIRasterizerState {
  RHICullMode cull_mode = RHICullMode::None;
  float depth_bias = 0.0f;
  float depth_bias_clamp = 0.0f;
  float depth_slope_scale = 0.0f;
  bool wireframe = false;
  bool front_ccw = false;
  bool depth_clip = true;
  bool line_aa = false;
  bool conservative_raster = false;
};

enum class RHICompareFunc {
  Never,
  Less,
  Equal,
  LessEqual,
  Greater,
  NotEqual,
  GreaterEqual,
  Always,
};

enum class RHIStencilOp {
  Keep,
  Zero,
  Replace,
  IncreaseClamp,
  DecreaseClamp,
  Invert,
  IncreaseWrap,
  DecreaseWrap,
};

struct RHIDepthStencilOp {
  RHIStencilOp stencil_fail = RHIStencilOp::Keep;
  RHIStencilOp depth_fail = RHIStencilOp::Keep;
  RHIStencilOp pass = RHIStencilOp::Keep;
  RHICompareFunc stencil_func = RHICompareFunc::Always;
};

struct RHIDepthStencilState {
  RHICompareFunc depth_func = RHICompareFunc::Always;
  bool depth_test = false;
  bool depth_write = true;
  RHIDepthStencilOp front;
  RHIDepthStencilOp back;
  bool stencil_test = false;
  uint8_t stencil_read_mask = 0xFF;
  uint8_t stencil_write_mask = 0xFF;
};

enum class RHIBlendFactor {
  Zero,
  One,
  SrcColor,
  InvSrcColor,
  SrcAlpha,
  InvSrcAlpha,
  DstAlpha,
  InvDstAlpha,
  DstColor,
  InvDstColor,
  SrcAlphaClamp,
  ConstantFactor,
  InvConstantFactor,
};

enum class RHIBlendOp {
  Add,
  Subtract,
  ReverseSubtract,
  Min,
  Max,
};

enum RHIColorWriteMaskBit {
  GfxColorWriteMaskR = 1,
  GfxColorWriteMaskG = 2,
  GfxColorWriteMaskB = 4,
  GfxColorWriteMaskA = 8,
  GfxColorWriteMaskAll = (GfxColorWriteMaskR | GfxColorWriteMaskG |
                          GfxColorWriteMaskB | GfxColorWriteMaskA),
};

using RHIColorWriteMask = uint8_t;

struct RHIBlendState {
  bool blend_enable = false;
  RHIBlendFactor color_src = RHIBlendFactor::One;
  RHIBlendFactor color_dst = RHIBlendFactor::One;
  RHIBlendOp color_op = RHIBlendOp::Add;
  RHIBlendFactor alpha_src = RHIBlendFactor::One;
  RHIBlendFactor alpha_dst = RHIBlendFactor::One;
  RHIBlendOp alpha_op = RHIBlendOp::Add;
  RHIColorWriteMask write_mask = GfxColorWriteMaskAll;
};

enum class RHIPrimitiveType {
  PointList,
  LineList,
  LineStrip,
  TriangleList,
  TriangleTrip,
};

enum class RHIPipelineType {
  Graphics,
  MeshShading,
  Compute,
  Raytracing,
};

class RHIShader;

struct RHIGraphicsPipelineDesc {
  RHIShader *vs = nullptr;
  RHIShader *ps = nullptr;

  RHIRasterizerState rasterizer_state;
  RHIDepthStencilState depthstencil_state;
  RHIBlendState blend_state[8];
  RHIFormat rt_format[8] = {RHIFormat::Unknown};
  RHIFormat depthstencil_format = RHIFormat::Unknown;
  RHIPrimitiveType primitive_type = RHIPrimitiveType::TriangleList;
};

struct RHIMeshShadingPipelineDesc {
  RHIShader *as = nullptr;
  RHIShader *ms = nullptr;
  RHIShader *ps = nullptr;

  RHIRasterizerState rasterizer_state;
  RHIDepthStencilState depthstencil_state;
  RHIBlendState blend_state[8];
  RHIFormat rt_format[8] = {RHIFormat::Unknown};
  RHIFormat depthstencil_format = RHIFormat::Unknown;
};

enum class RHIFilter {
  Point,
  Linear,
};

enum class RHISamplerAddressMode {
  Repeat,
  MirroredRepeat,
  ClampToEdge,
  ClampToBorder,
};

struct RHISamplerDesc {
  RHIFilter min_filter = RHIFilter::Point;
  RHIFilter mag_filter = RHIFilter::Point;
  RHIFilter mip_filter = RHIFilter::Point;
  RHISamplerAddressMode address_u = RHISamplerAddressMode::Repeat;
  RHISamplerAddressMode address_v = RHISamplerAddressMode::Repeat;
  RHISamplerAddressMode address_w = RHISamplerAddressMode::Repeat;
  float mip_bias = 0.0f;
  bool enable_anisotropy = false;
  float max_anisotropy = 1.0f;
  bool enable_compare = false;
  RHICompareFunc compare_func = RHICompareFunc::Always;
  float min_lod = -FLT_MAX;
  float max_lod = FLT_MAX;
  float border_color[4] = {};
};