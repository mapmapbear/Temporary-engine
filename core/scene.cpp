#include "scene.h"
#include <iostream>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

void Scene::Load(std::string fileName) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  tinygltf::Model model;
  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res)
    std::cout << "Failed to load glTF: " << fileName << std::endl;
  else
    std::cout << "Loaded glTF: " << fileName << std::endl;
  for (auto &gltfBuffer : model.buffers) {
    auto name = gltfBuffer.name;
  }
}