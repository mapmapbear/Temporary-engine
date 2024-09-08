#pragma once

#include "renderer/renderer.h"
#include "world.h"

class Engine {
public:
  static Engine *GetInstance();

  void Init(const std::string &work_path, void *window_handle,
            uint32_t window_width, uint32_t window_height);
  void Shut();
  void Tick();
  void *GetWindowHandle() const { return m_windowHandle; }
  World *GetWorld() const { return m_pWorld.get(); }
  Renderer *GetRenderer() const { return m_pRenderer.get(); }
  void CallWindowResize(uint32_t width, uint32_t height);
  const std::string &GetWorkPath() const { return m_workPath; }
  const std::string &GetAssetPath() const { return m_assetPath; }
  const std::string &GetShaderPath() const { return m_shaderPath; }

private:
  void LoadEngineConfig();

private:
  std::unique_ptr<Renderer> m_pRenderer;
  std::unique_ptr<World> m_pWorld;
  uint64_t m_lastFrameTime = 0;
  void *m_windowHandle = nullptr;
  std::string m_workPath;
  std::string m_assetPath;
  std::string m_shaderPath;
  bool m_enableVsync = false;
};