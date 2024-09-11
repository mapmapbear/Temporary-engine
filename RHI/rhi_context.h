#pragma once

#include "rhi_define.h"
#include "rhi_device.h"
#include "rhi_buffer.h"
#include "rhi_texture.h"
#include "rhi_command_list.h"
#include "rhi_fence.h"
#include "rhi_shader.h"
#include "rhi_descriptor.h"
#include "rhi_pipeline_state.h"
#include "rhi_swapchain.h"

RHIDevice* CreateGfxDevice(const RHIDeviceDesc& desc);
uint32_t GetFormatRowPitch(RHIFormat format, uint32_t width);
uint32_t GetFormatBlockWidth(RHIFormat format);
uint32_t GetFormatBlockHeight(RHIFormat format);

class RenderEvent
{
public:
    RenderEvent(RHICommandList* pCommandList, const std::string& event_name) :
        m_pCommandList(pCommandList)
    {
        pCommandList->BeginEvent(event_name);
    }

    ~RenderEvent()
    {
        m_pCommandList->EndEvent();
    }

private:
    RHICommandList* m_pCommandList;
};

#define RENDER_EVENT(pCommandList, event_name) RenderEvent __render_event__(pCommandList, event_name)