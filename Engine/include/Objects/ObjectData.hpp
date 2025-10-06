#ifndef OBJECTDATA_HPP
#define OBJECTDATA_HPP

#include <cstdint>
#include <sys/cdefs.h>

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "shader.hpp"

namespace Engine {
namespace Objects {

struct __attribute__((packed)) ObjectData {
  uint32_t uuid;
  float color[3];
  float model[16];
  Shader *shader;
};

struct PolyData {
  uint32_t uuid;
  Math::Vector<3> color;
  Math::Vector<3> borderColor;
  float borderSize;

  uint32_t VAO;
  uint32_t count;
  Shader *shader;
};

} // namespace Objects

} // namespace Engine

#endif // OBJECTDATA_HPP
