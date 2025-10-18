#include "engine.hpp"
#include "Math/Matrix.hpp"
#include "Objects/ObjectData.hpp"
#include "Objects/ObjectManager.hpp"

#include "Objects/ObjectUUID.hpp"
#include "Solvers/Instanced.hpp"
#include "Wrappers/Line.hpp"
#include "Wrappers/Point.hpp"

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

  m_instance->m_windowSize[0] = windowSize[0];
  m_instance->m_windowSize[1] = windowSize[1];
  m_instance->m_objManager.setSolver(new Solver::Instanced);

  std::filesystem::path shadersRoot =
      std::filesystem::current_path() / "assets" / "shaders";
  if (!std::filesystem::exists(shadersRoot)) {
    shadersRoot = std::filesystem::current_path() / ".." / "assets" / "shaders";
  }

  m_instance->m_shaderManager.init(shadersRoot);

  glGenBuffers(1, &m_instance->uboMatrices);
  glGenFramebuffers(1, &m_instance->m_fboID);
  glGenTextures(1, &m_instance->m_colorTextureID);
  glGenTextures(1, &m_instance->m_idTextureID);
  glGenRenderbuffers(1, &m_instance->m_rboDepthStencil);

  if (!m_instance->resize((double)windowSize[0], (double)windowSize[1])) {
    glDeleteFramebuffers(1, &m_instance->m_fboID);
    glDeleteTextures(1, &m_instance->m_colorTextureID);
    glDeleteTextures(1, &m_instance->m_idTextureID);
    glDeleteRenderbuffers(1, &m_instance->m_rboDepthStencil);
  }

  return m_instance.get();
}

Engine *Engine::get() { return m_instance.get(); }

bool Engine::resize(double w, double h) {
  unsigned int rboDepthStencil;
  m_windowSize[0] = w;
  m_windowSize[1] = h;

  glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

  glBindTexture(GL_TEXTURE_2D, m_colorTextureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_windowSize[0], m_windowSize[1], 0,
               GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_colorTextureID, 0);

  glBindTexture(GL_TEXTURE_2D, m_idTextureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, m_windowSize[0], m_windowSize[1], 0,
               GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         m_idTextureID, 0);

  unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);

  glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepthStencil);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_windowSize[0],
                        m_windowSize[1]);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, m_rboDepthStencil);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
              << std::endl;
    return false;
  }

  glViewport(0, 0, w, h);
  Math::Matrix<4, 4> proj =
      Math::Matrix<4, 4>::Ortho(0, m_windowSize[0], 0, m_windowSize[1], -1, 1);

  glBindBuffer(GL_UNIFORM_BUFFER, m_instance->uboMatrices);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_instance->uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, proj.size(), &proj[0], GL_STATIC_DRAW);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  return true;
}

void Engine::draw() {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
  glClearColor(0, 0, 0, 255);
  unsigned int clearID[2] = {0, 0};
  glClearBufferuiv(GL_COLOR, 1, clearID); // Clear attachment 1
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glBindBuffer(GL_UNIFORM_BUFFER, m_instance->uboMatrices);
  m_objManager.draw();
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboID);

  glReadBuffer(GL_COLOR_ATTACHMENT0);

  glBlitFramebuffer(0, 0, m_windowSize[0], m_windowSize[1], // Source rectangle
                    0, 0, m_windowSize[0],
                    m_windowSize[1],     // Destination rectangle
                    GL_COLOR_BUFFER_BIT, // Buffers to copy
                    GL_NEAREST           // Filter to use if scaling
  );

  // Unbind the read framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

int Engine::getType(Objects::ObjectUUID::UUID id) {
  if (id == 0) {
    return -1;
  }

  return m_objManager.type(id);
}

Objects::ObjectUUID::UUID Engine::lookupObjectUUID(int x, int y) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fboID);
  glReadBuffer(GL_COLOR_ATTACHMENT1);

  unsigned int pixel_data[2];

  glReadPixels(x, y, // The x and y coordinates of the pixel
               1, 1, // The width and height of the rectangle to read (just 1x1)
               GL_RED_INTEGER,  // The format must match the texture's format
               GL_UNSIGNED_INT, // The type must match the texture's type
               &pixel_data[0]   // A pointer to the buffer to store the data
  );

  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  unsigned int objectID = pixel_data[0];

  return objectID;
}

std::variant<Objects::PolyData *, Objects::ObjectData *>
Engine::get(Objects::ObjectUUID::UUID &id) {
  return m_objManager.get(id);
}

Point Engine::createPoint(Math::Vector<2> pos, Math::Vector<3> color,
                          float radius, Shader *shader) {
  Objects::ObjectData data;

  memcpy(data.color, &color[0], color.size());

  pos[0] -= radius * 0.5f;
  pos[1] -= radius * 0.5f;
  Math::Matrix<4, 4> model;
  model.model(pos, {radius, radius});
  memcpy(data.model, &model[0], model.size());

  if (!shader) {
    data.shader = &m_shaderManager.at("Point");
  } else {
    data.shader = shader;
  }

  return Point(
      pos, color, radius,
      m_objManager.add(Objects::ObjectManager::Types::POINT, std::move(data)),
      m_objManager);
}

Line Engine::createLine(Math::Vector<2> pos0, Math::Vector<2> pos1,
                        Math::Vector<3> color, float stroke, Shader *shader) {
  if (!shader) {
    shader = &m_shaderManager.at("Line");
  }

  return Line(pos0, pos1, color, stroke, shader, m_objManager);
}

Poly Engine::createPoly(std::vector<Math::Vector<2>> &verts,
                        Math::Vector<3> color, Math::Vector<3> borderColor,
                        float borderSize, Shader *shader) {
  if (!shader) {
    shader = &m_shaderManager.at("Poly");
  }

  return Poly(verts, color, borderColor, borderSize, shader, m_objManager);
}

void Engine::remove(Objects::ObjectUUID::UUID id) {
  std::variant<Objects::PolyData *, Objects::ObjectData *> s =
      m_objManager.get(id);

  m_objManager.remove(id);
}

void Engine::clear() { m_objManager.clear(); }

void Engine::setWinSize(Math::Vector<2, float> m_windowSize) {
  resize(m_windowSize[0], m_windowSize[1]);
}

uint32_t Engine::drawCalls() { return m_objManager.drawCalls(); }
uint32_t Engine::entities() { return m_objManager.entities(); }
Objects::ObjectManager::ObjectCount Engine::count() {
  return m_objManager.count();
}
Math::Vector<2, uint32_t> Engine::winSize() { return m_windowSize; }

} // namespace Engine
