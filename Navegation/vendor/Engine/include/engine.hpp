#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <glad/glad.h>
#include <memory>

#include "Math/Vector.hpp"
#include "Objects/ObjectManager.hpp"

#include "Objects/ObjectUUID.hpp"
#include "Wrappers/Line.hpp"
#include "Wrappers/Point.hpp"
#include "Wrappers/Poly.hpp"
#include "engine_api.hpp"
#include "shader.hpp"

namespace Engine {
// Singleton
class ENGINE_API Engine {
public:
  static Engine *init(GLADloadproc proc, Math::Vector<2, uint32_t> &windowSize);
  static Engine *get();

  bool resize(double w, double h);

  void update(double dt);
  void draw();

  Objects::ObjectUUID::UUID lookupObjectUUID(int x, int y);
  int getType(Objects::ObjectUUID::UUID id);

  std::variant<Objects::PolyData *, Objects::ObjectData *>
  get(Objects::ObjectUUID::UUID &id);

  Point createPoint(Math::Vector<2> pos, Math::Vector<3> color, float radius,
                    Shader *shader = nullptr);
  Line createLine(Math::Vector<2> pos0, Math::Vector<2> pos1,
                  Math::Vector<3> color, float stroke,
                  Shader *shader = nullptr);
  Poly createPoly(std::vector<Math::Vector<2>> &verts, Math::Vector<3> color,
                  Math::Vector<3> borderColor, float borderSize,
                  Shader *shader = nullptr);

  void remove(Objects::ObjectUUID::UUID id);
  void clear();

  void setWinSize(Math::Vector<2, float> m_windowSize);

  Math::Vector<2, uint32_t> winSize();

  Engine(const Engine &) = delete;
  Engine &operator=(const Engine &) = delete;

  uint32_t drawCalls();
  uint32_t entities();
  Objects::ObjectManager::ObjectCount count();

private:
  Engine() = default;

  // Variables

public:
private:
  uint32_t uboMatrices;
  Math::Vector<2, uint32_t> m_windowSize;
  Objects::ObjectManager m_objManager;
  inline static std::unique_ptr<Engine> m_instance = nullptr;
  ShaderManager m_shaderManager;
  Shader *currentShader;

  uint32_t m_fboID = 0;
  uint32_t m_colorTextureID = 0;
  uint32_t m_idTextureID = 0;
  uint32_t m_rboDepthStencil = 0;
};

} // namespace Engine

#endif
