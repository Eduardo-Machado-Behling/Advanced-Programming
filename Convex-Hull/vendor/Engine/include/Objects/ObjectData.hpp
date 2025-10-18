#ifndef OBJECTDATA_HPP
#define OBJECTDATA_HPP

#include <cstdint>

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "shader.hpp"

namespace Engine {
namespace Objects {

#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif

struct ObjectData {
  uint32_t uuid;
  float color[3];
  float model[16];
  Shader *shader;
}
// GCC and Clang use __attribute__((packed)).
#if defined(__GNUC__) || defined(__clang__)
__attribute__((packed))
#endif
;

// Restore the original packing alignment for MSVC.
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

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
