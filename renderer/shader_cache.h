#pragma once

#include "../RHI/rhi_context.h"
#include <unordered_map>
#include <memory>

namespace std
{
    template <>
    struct hash<RHIShaderDesc>
    {
        size_t operator()(const RHIShaderDesc& desc) const
        {
            std::string s = desc.file + desc.entry_point + desc.profile;
            for (size_t i = 0; i < desc.defines.size(); ++i)
            {
                s += desc.defines[i];
            }

            return std::hash<std::string>{}(s);
        }
    };
}

class Renderer;

class ShaderCache
{
public:
    ShaderCache(Renderer* pRenderer);

    RHIShader* GetShader(const std::string& file, const std::string& entry_point, const std::string& profile, const std::vector<std::string>& defines);

private:
    RHIShader* CreateShader(const std::string& file, const std::string& entry_point, const std::string& profile, const std::vector<std::string>& defines);

private:
    Renderer* m_pRenderer;
    std::unordered_map<RHIShaderDesc, std::unique_ptr<RHIShader>> m_cachedShaders;
};