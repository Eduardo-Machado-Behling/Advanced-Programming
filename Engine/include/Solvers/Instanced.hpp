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

  void operator()(size_t type, std::vector<Objects::ObjectData> &data,
                  bool update) override {
    const uint32_t amount = data.size();

    if (amount == 0) {
      return;
    }

    glBindVertexArray(Objects::ObjectVAO::get());
    glBindBuffer(GL_ARRAY_BUFFER, VBO[type]);

    if (update) {
      printf("Type: %d, update: %d\n", (int)type, update);
      const uint32_t size = amount * sizeof(data[0]);

      if (this->size < size) {
        glBufferData(GL_ARRAY_BUFFER, size, data.data(), GL_STATIC_DRAW);
      }

      for (size_t i = 0; i < 19; i++) {
        printf("%f ", ((float *)(data.data()))[i]);
      }
      putchar('\n');

    }

	size_t stride = sizeof(data[0]);
	size_t offset = 0;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)offset);
	glVertexAttribDivisor(1, 1);
	offset += 3 * sizeof(float);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
	glVertexAttribDivisor(2, 1);
	offset += 4 * sizeof(float);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
	glVertexAttribDivisor(3, 1);
	offset += 4 * sizeof(float);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
	glVertexAttribDivisor(4, 1);
	offset += 4 * sizeof(float);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void *)offset);
	glVertexAttribDivisor(5, 1);

    data[0].shader->bind();
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, amount);
    data[0].shader->unbind();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void operator()(std::vector<Objects::PolyData> &polys, bool update) override {
    for (auto &poly : polys) {
      glBindVertexArray(poly.VAO);
      poly.shader->bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, poly.count);
      poly.shader->unbind();
    }
  }

private:
  uint32_t VBO[3];
  uint32_t size = 0;
};

} // namespace Solver

} // namespace Engine

#endif // INSTANCED_HPP
