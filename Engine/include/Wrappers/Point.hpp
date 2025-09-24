#ifndef POINT_HPP
#define POINT_HPP

#include "Math/Matrix.hpp"
#include "Objects/ObjectData.hpp"
#include "Objects/ObjectManager.hpp"
#include "Objects/ObjectUUID.hpp"
#include <cstring>
namespace Engine {
class Point {
public:
  Point(Math::Vector<2> pos, Math::Vector<3> color, float radius,
        Objects::ObjectUUID::UUID id, Objects::ObjectManager &manager)
      : m_pos(pos), m_color(color), m_radius(radius), m_id(id),
        m_manager(manager) {}

  void setPos(Math::Vector<2> newPos) {
    if (newPos == m_pos) {
      return;
    }

    m_pos = newPos;
    updateMatrix();
  }

  Math::Vector<2> getPos() { return m_pos; }

  void setRadius(float rad) {
    if (m_radius == rad) {
      return;
    }

    m_radius = rad;
    updateMatrix();
  }

  float getRadius() { return m_radius; }

  void setColor(Math::Vector<3> newColor) {
    if (newColor == m_color) {
      return;
    }

    m_color = newColor;
    updateColor();
  }

  Math::Vector<3> getColor() { return m_color; }

  Objects::ObjectUUID::UUID getID() { return m_id; }

private:
  void updateMatrix() {
    m_model.identity();
    m_model.model(m_pos, {m_radius, m_radius});
    Objects::ObjectData *data = std::get<1>(m_manager.get(m_id));
    memcpy(data->model, &m_model[0], m_model.size());
  }

  void updateColor() {
    Objects::ObjectData *data = std::get<1>(m_manager.get(m_id));
    memcpy(data->color, &m_color[0], m_color.size());
  }

  Math::Matrix<4, 4> m_model;
  Math::Vector<2> m_pos;
  Math::Vector<3> m_color;
  float m_radius;

  Objects::ObjectUUID::UUID m_id;
  Objects::ObjectManager &m_manager;
};

} // namespace Engine

#endif
