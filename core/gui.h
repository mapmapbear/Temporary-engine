#pragma once

#include "../RHI/rhi_context.h"

class GUI
{
public:
    GUI();
    ~GUI();

    bool Init();
    void NewFrame();
    void Render();

private:
    RHIPipelineState* m_pPSO = nullptr;
};