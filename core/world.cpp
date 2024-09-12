#include "world.h"
#include "scene.h"

World::World() {
  m_pCamera = std::make_unique<Camera>();
  m_pGUI = std::make_unique<GUI>();
  m_pGUI->Init();
}

void World::LoadScene(std::string sceneName) {
  Scene scene_node;
  scene_node.Load(sceneName);
}

void World::Tick(float delta_time) {
  TickGUI();

  m_pCamera->Tick(delta_time);
}

void World::TickGUI() {
  m_pGUI->NewFrame();

  // todo : imgui code goes here
}
