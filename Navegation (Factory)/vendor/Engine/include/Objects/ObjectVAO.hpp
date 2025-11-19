#ifndef OBJECTVAO_HPP
#define OBJECTVAO_HPP

#include <glad/glad.h>

#include <cstdint>
#include <memory>

namespace Engine {
namespace Objects {

class ObjectVAO {
public:
  static uint32_t get() {
    if (!m_instance) {
      m_instance.reset(new ObjectVAO());
    }

    return m_instance->m_VAO;
  }

private:
  ObjectVAO() {
    // clang-format off
    float data[] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
    };
    // clang-format on

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  inline static std::unique_ptr<ObjectVAO> m_instance = nullptr;
  uint32_t m_VAO;
  uint32_t m_VBO;
};

} // namespace Objects

} // namespace Engine

#endif // OBJECTVAO_HPP
