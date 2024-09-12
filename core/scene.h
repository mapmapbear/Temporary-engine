#pragma once
#include "../utils/fmath.h"
#include "RHI/rhi_buffer.h"
#include <string>
#include <vector>


// struct SubMesh {
//   RHIBuffer posStream;
//   RHIBuffer uvStream;
//   RHIBuffer normalStream;
//   RHIBuffer vcolorStream;
// };

class Scene {
public:
  Scene() = default;
  void Load(std::string fileName);

private:
//   std::vector<SubMesh> meshNodes;
};