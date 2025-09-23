#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <glad/glad.h>
#include <memory>

#include "Math/Vector.hpp"
#include "Objects/ObjectManager.hpp"

#include "engine_api.hpp"
#include "shader.hpp"

namespace Engine {
// Singleton
class ENGINE_API Engine {
public:
  static Engine *init(GLADloadproc proc, Math::Vector<2, uint32_t> &windowSize);
  static Engine *get();

  void resize(double w, double h);

  void update(double dt);
  void draw();

  std::variant<Objects::ObjectData *, Objects::PolyData *>
  get(Objects::ObjectManager::ID &id);

  Objects::ObjectManager::ID createPoint(Math::Vector<2> pos,
                                         Math::Vector<3> color, float radius,
                                         Shader *shader = nullptr);
  Objects::ObjectManager::ID createLine(Math::Vector<2> pos0,
                                        Math::Vector<2> pos1,
                                        Math::Vector<3> color, float stroke,
                                        Shader *shader = nullptr);
  Objects::ObjectManager::ID createPoly(std::vector<Math::Vector<2>> &verts,
                                        Math::Vector<3> color,
                                        Shader *shader = nullptr);

  Math::Vector<2, uint32_t> winSize();

  Engine(const Engine &) = delete;
  Engine &operator=(const Engine &) = delete;

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
};

} // namespace Engine

#endif
