#ifndef INSTANCED_HPP
#define INSTANCED_HPP

#include <cstdint>
#include <vector>

#include <glad/glad.h>

#include "Objects/ObjectData.hpp"
#include "Objects/ObjectManager.hpp"
#include "Objects/ObjectVAO.hpp"

namespace Engine {
namespace Solver {
class Instanced : public Objects::ObjectManager::Solver {
public:
  Instanced() { glGenBuffers(3, VBO); }

  uint32_t getDrawCalls() override {
    uint32_t d = m_drawCalls;
    m_drawCalls = 0;
    return d;
  }

  void operator()(size_t type, std::vector<Objects::ObjectData> &data,
                  bool update) override {
    const uint32_t amount = data.size();

    if (amount == 0) {
      return;
    }

    glBindVertexArray(Objects::ObjectVAO::get());
    glBindBuffer(GL_ARRAY_BUFFER, VBO[type]);

    if (update) {
      const uint32_t size = amount * sizeof(data[0]);

      if (this->size < size) {
        glBufferData(GL_ARRAY_BUFFER, size, data.data(), GL_STATIC_DRAW);
      }
    }

    size_t stride = sizeof(data[0]);
    size_t offset = 0;
    size_t loc = 1;
    glEnableVertexAttribArray(loc);
    glVertexAttribIPointer(loc, 1, GL_UNSIGNED_INT, stride, (void *)offset);
    glVertexAttribDivisor(loc, 1);
    loc++;
    offset += 1 * sizeof(uint32_t);

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    glVertexAttribDivisor(loc, 1);
    loc++;
    offset += 3 * sizeof(float);

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    glVertexAttribDivisor(loc, 1);
    loc++;
    offset += 4 * sizeof(float);

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    glVertexAttribDivisor(loc, 1);
    loc++;
    offset += 4 * sizeof(float);

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    glVertexAttribDivisor(loc, 1);
    loc++;
    offset += 4 * sizeof(float);

    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
    glVertexAttribDivisor(loc, 1);

    data[0].shader->bind();
    m_drawCalls++;
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount);
    data[0].shader->unbind();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void operator()(std::vector<Objects::PolyData> &polys, bool update) override {
    if (polys.empty()) {
      return;
    }

    for (auto &poly : polys) {
      glBindVertexArray(poly.VAO);
      poly.shader->bind();

      m_drawCalls++;
      glDrawElements(GL_TRIANGLES, poly.count, GL_UNSIGNED_INT, 0);

      poly.shader->set("UUID", poly.uuid);
      poly.shader->set("fillColor", poly.color);
      poly.shader->set("borderColor", poly.borderColor);
      poly.shader->set("borderSize", poly.borderSize);

      poly.shader->unbind();
    }
  }

private:
  uint32_t VBO[3];
  uint32_t size = 0;
  uint32_t m_drawCalls = 0;
};

} // namespace Solver

} // namespace Engine

#endif // INSTANCED_HPP
