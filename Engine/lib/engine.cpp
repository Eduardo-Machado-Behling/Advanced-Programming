#include "engine.hpp"
#include "Math/Matrix.hpp"
#include "Objects/ObjectData.hpp"
#include "Objects/ObjectManager.hpp"

#include "Solvers/Instanced.hpp"

#include <glad/glad.h>

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <glad/glad.h>

namespace Engine {
Engine *Engine::init(GLADloadproc proc, Math::Vector<2, uint32_t> &windowSize) {
  if (gladLoadGLLoader(proc)) {
    m_instance.reset(new Engine());
  }

  glViewport(0, 0, windowSize[0], windowSize[1]);

  m_instance->m_windowSize[0] = windowSize[0];
  m_instance->m_windowSize[1] = windowSize[1];
  m_instance->m_objManager.setSolver(new Solver::Instanced);

  Math::Matrix<4, 4> proj =
      Math::Matrix<4, 4>::Ortho(0, windowSize[0], 0, windowSize[1], -1, 1);
  glGenBuffers(1, &m_instance->uboMatrices);
  glBindBuffer(GL_UNIFORM_BUFFER, m_instance->uboMatrices);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_instance->uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, proj.size(), &proj[0], GL_STATIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  std::cout << "proj = Math::Matrix<4,4>::Ortho(0, " << windowSize[0] << ", 0, "
            << windowSize[1] << ", -1, 1): \n";
  for (size_t i = 0; i < 16; i++) {
    std::cout << proj[i] << ((i + 1) % 4 == 0 ? '\n' : ' ');
  }
  std::cout << "\nsize: " << proj.size() << '\n';

  m_instance->m_shaderManager.init(std::filesystem::current_path() / "assets" /
                                   "shaders");

  return m_instance.get();
}

Engine *Engine::get() { return m_instance.get(); }

void Engine::draw() {
  glClearColor(0, 0, 0, 255);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindBuffer(GL_UNIFORM_BUFFER, m_instance->uboMatrices);
  m_objManager.draw();
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::variant<Objects::ObjectData *, Objects::PolyData *>
Engine::get(Objects::ObjectManager::ID &id) {
  return m_objManager.get(id);
}

Objects::ObjectManager::ID Engine::createPoint(Math::Vector<2> pos,
                                               Math::Vector<3> color,
                                               float radius, Shader *shader) {
  Objects::ObjectData data;

  memcpy(data.color, &color[0], color.size());

  pos[0] -= radius * 0.5f;
  pos[1] -= radius * 0.5f;
  Math::Matrix<4, 4> model;
  model.model(pos[0], {radius, radius});
  memcpy(data.model, &model[0], model.size());

  model.print();
  color.print();

  if (!shader) {
    data.shader = &m_shaderManager.at("Point");
  } else {
    data.shader = shader;
  }

  return m_objManager.add(Objects::ObjectManager::Types::POINT,
                          std::move(data));
}

Objects::ObjectManager::ID Engine::createLine(Math::Vector<2> pos0,
                                              Math::Vector<2> pos1,
                                              Math::Vector<3> color,
                                              float stroke, Shader *shader) {
  Objects::ObjectData data;

  Math::Vector<2> pos = pos0;
  float width = (pos1 - pos0).mag();
  pos0.norm();
  pos1.norm();

  std::cout << "pos:\n";
  pos0.print();
  pos1.print();
  float angle = pos0.angle({1, 0});
  std::cout << "angle: " << angle << '\n';

  memcpy(data.color, &color[0], color.size());
  Math::Matrix<4, 4> model;
  model.model(pos, {width, stroke}, angle);
  memcpy(data.model, &model[0], model.size());

  if (!shader) {
    data.shader = &m_shaderManager.at("Line");
  } else {
    data.shader = shader;
  }

  return m_objManager.add(Objects::ObjectManager::Types::LINE, std::move(data));
}

Objects::ObjectManager::ID
Engine::createPoly(std::vector<Math::Vector<2>> &verts, Math::Vector<3> color,
                   Shader *shader) {
  Objects::PolyData data;

  if (!shader) {
    data.shader = &m_shaderManager.at("Poly");
  } else {
    data.shader = shader;
  }

  data.count = verts.size();
  memcpy(data.color, &color[0], color.size());

  glGenVertexArrays(1, &data.VAO);
  glBindVertexArray(data.VAO);

  uint32_t vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), verts.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return m_objManager.add(std::move(data));
}

Math::Vector<2, uint32_t> Engine::winSize() { return m_windowSize; }

} // namespace Engine
