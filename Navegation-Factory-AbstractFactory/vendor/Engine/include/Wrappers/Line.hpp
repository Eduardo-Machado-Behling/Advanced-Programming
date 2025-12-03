#ifndef LINE_HPP
#define LINE_HPP

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "Objects/ObjectData.hpp"
#include "Objects/ObjectManager.hpp"
#include "Objects/ObjectUUID.hpp"
#include "shader.hpp"

#include <cstring>
#include <utility>

namespace Engine {
class Line {
public:
  Line(Line &&line) = default;
  Line(Math::Vector<2> pos0, Math::Vector<2> pos1, Math::Vector<3> color,
       float stroke, Shader *shader, Objects::ObjectManager &manager)
      : m_verts({pos0, pos1}), m_stroke(stroke), m_color(color),
        m_manager(manager) {
    Objects::ObjectData data;
    data.shader = shader;
    updateModel(data);
    updateColor(data);

    m_id = manager.add(Objects::ObjectManager::Types::LINE, std::move(data));
  }

  const std::pair<Math::Vector<2>, Math::Vector<2>> &getVerts() {
    return m_verts;
  }

  void setVerts(Math::Vector<3> color) {
    if (m_color == color)
      return;

    m_color = color;
    Objects::ObjectData *data = std::get<1>(m_manager.get(m_id));
    if (data)
      updateColor(*data);
  }

  void setVerts(Math::Vector<2> pos0, Math::Vector<2> pos1) {
    if (pos0 == std::get<0>(m_verts) && pos1 == std::get<1>(m_verts))
      return;

    m_verts.first = pos0;
    m_verts.second = pos1;

    Objects::ObjectData *data = std::get<1>(m_manager.get(m_id));
    if (data)
      updateModel(*data);
  }

private:
  void updateModel(Objects::ObjectData &data) {
    Math::Vector<2> pos = std::get<0>(m_verts);
    Math::Vector<2> dir = std::get<1>(m_verts) - pos;
    float width = dir.mag();
    float angle = dir.angle();

    Math::Matrix<4, 4> model;
    model.model(pos, {width, m_stroke}, angle);
    memcpy(data.model, &model[0], model.size());
  }

  void updateColor(Objects::ObjectData &data) {
    memcpy(data.color, &m_color[0], m_color.size());
  }

  std::pair<Math::Vector<2>, Math::Vector<2>> m_verts;
  Math::Vector<3> m_color;
  float m_stroke;

  Objects::ObjectUUID::UUID m_id;
  Objects::ObjectManager &m_manager;
};

} // namespace Engine

#endif
