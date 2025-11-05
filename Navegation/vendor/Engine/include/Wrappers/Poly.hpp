#ifndef POLY_HPP
#define POLY_HPP

#include "Math/Vector.hpp"
#include "Objects/ObjectData.hpp"
#include "Objects/ObjectManager.hpp"
#include "Objects/ObjectUUID.hpp"
#include "shader.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>
namespace Engine {
class Poly {
public:
  Poly(std::vector<Math::Vector<2>> verts, Math::Vector<3> color,
       Math::Vector<3> borderColor, float borderSize, Shader *shader,
       Objects::ObjectManager &manager)
      : m_verts(verts), m_indices({0, 1, 2}), m_borderColor(borderColor),
        m_borderSize(borderSize), m_color(color), m_manager(&manager) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    mesh();

    Objects::PolyData data;
    data.shader = shader;
    setup(data);
    m_id = manager.add(std::move(data));
  }

  Poly(Poly &&other) noexcept
      : m_verts(std::move(other.m_verts)),
        m_indices(std::move(other.m_indices)), m_color(other.m_color),
        m_borderColor(other.m_borderColor), m_borderSize(other.m_borderSize),
        VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), m_id(other.m_id),
        m_manager(other.m_manager) {

    // Invalidate the moved-from object
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.m_id = 0;
  }

  Poly &operator=(Poly &&other) noexcept {
    if (this != &other) {
      // Free current GL resources
      if (m_id != 0) {
        if (VBO)
          glDeleteBuffers(1, &VBO);
        if (EBO)
          glDeleteBuffers(1, &EBO);
        if (VAO)
          glDeleteVertexArrays(1, &VAO);
      }

      m_verts = std::move(other.m_verts);
      m_indices = std::move(other.m_indices);
      m_color = other.m_color;
      m_borderColor = other.m_borderColor;
      m_borderSize = other.m_borderSize;
      VAO = other.VAO;
      VBO = other.VBO;
      EBO = other.EBO;
      m_id = other.m_id;
      m_manager = other.m_manager;

      other.VAO = 0;
      other.VBO = 0;
      other.EBO = 0;
      other.m_id = 0;
    }
    return *this;
  }

  Poly(const Poly &) = delete;
  Poly &operator=(const Poly &) = delete;

  ~Poly() {
    if (m_id != 0) {
      if (VAO)
        m_manager->deleteVAO(1, &VAO);
      if (VBO)
        m_manager->deleteBuffers(1, &VBO);
      if (EBO)
        m_manager->deleteBuffers(1, &VBO);
    }
  }

  const std::vector<Math::Vector<2>> &getVerts() { return m_verts; }
  const std::vector<uint32_t> &getIndices() { return m_indices; }

  void removeVert(size_t i) {
    m_verts.erase(std::next(m_verts.begin(), i));

    m_indices.clear();
    m_indices.push_back(0);
    m_indices.push_back(1);
    m_indices.push_back(2);
    mesh();

    setup(*std::get<0>(m_manager->get(m_id)));
  }

  void addVert(Math::Vector<2> vert) {
    m_verts.push_back(vert);

    mesh();

    setup(*std::get<0>(m_manager->get(m_id)));
  }

private:
  void mesh() {
    m_indices.clear();
    if (m_verts.size() < 3)
      return;

    // Create a fan of triangles from the first vertex
    for (size_t i = 1; i < m_verts.size() - 1; ++i) {
      m_indices.push_back(0); // The anchor vertex
      m_indices.push_back(i);
      m_indices.push_back(i + 1);
    }
  }

  void setup(Objects::PolyData &data) {
    data.count = m_indices.size();
    data.color = m_color;
    data.borderColor = m_borderColor;
    data.borderSize = m_borderSize;
    data.VAO = VAO;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_verts.size() * sizeof(m_verts[0]),
                 m_verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_indices.size() * sizeof(m_indices[0]), m_indices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  std::vector<Math::Vector<2>> m_verts;
  std::vector<uint32_t> m_indices;

  Math::Vector<3> m_color;
  Math::Vector<3> m_borderColor;
  float m_borderSize;
  uint32_t VAO, VBO, EBO;

  Objects::ObjectUUID::UUID m_id;
  Objects::ObjectManager *m_manager;
};

} // namespace Engine

#endif
