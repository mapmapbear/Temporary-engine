#include "engine.h"
#include "SimpleIni.h"
#include "utils/log.h"
#define SOKOL_IMPL
#include "sokol_time.h"

Engine *Engine::GetInstance() {
  static Engine engine;
  return &engine;
}

void Engine::Init(const std::string &work_path, void *window_handle,
                  uint32_t window_width, uint32_t window_height) {
  m_windowHandle = window_handle;
  m_workPath = work_path;
  LoadEngineConfig();
  m_pRenderer = std::make_unique<Renderer>();
  m_pRenderer->CreateDevice(window_handle, window_width, window_height,
                            m_enableVsync);
  m_pWorld = std::make_unique<World>();
  m_pWorld->LoadScene("cube.gltf");
  Camera *camera = m_pWorld->GetCamera();
  camera->SetPerpective((float)window_width / window_height, (float)90,
                        (float)0.1, (float)100.0);
  stm_setup();
}

void Engine::Shut() {}

void Engine::Tick() {
  float frame_time = (float)stm_sec(stm_laptime(&m_lastFrameTime));

  m_pWorld->Tick(frame_time);

  m_pRenderer->RenderFrame();
  // m_pWorld->GetGUI()->Render();
}

void Engine::LoadEngineConfig() {
  std::string ini_file = m_workPath + "RealEngine.ini";

  CSimpleIniA Ini;
  SI_Error error = Ini.LoadFile(ini_file.c_str());
  if (error != SI_OK) {
    RE_LOG("Failed to load RealEngine.ini !");
    return;
  }

  m_assetPath = m_workPath + Ini.GetValue("RealEngine", "AssetPath");
  m_shaderPath = m_workPath + Ini.GetValue("RealEngine", "ShaderPath");
  m_enableVsync = Ini.GetBoolValue("RealEngine", "VSync");
}

void Engine::CallWindowResize(uint32_t width, uint32_t height) {
  m_pRenderer->OnWindowResize(width, height);
  m_pWorld->GetCamera()->OnWindowResize(width, height);
}