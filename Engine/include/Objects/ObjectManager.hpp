#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

#include <memory>
#include <variant>
#include <vector>

#include "Objects/ObjectData.hpp"

namespace Engine {
namespace Objects {

class ObjectManager {
public:
  struct Solver {
    virtual void operator()(size_t i, std::vector<ObjectData> &data, bool update) = 0;
    virtual void operator()(std::vector<PolyData> &data, bool update) = 0;
  };

  enum class Types {
    POINT,
    LINE,
    POLY,
  };

  class ID {
    ID() : type(UINT32_MAX), i(UINT32_MAX) {}
    ID(uint32_t t, uint32_t i) : type(t), i(i) {}

    uint32_t type, i;
    friend class ObjectManager;
  };

  void draw() {
    (*solver)(0, data.points, data.pointsUpdate);
	data.pointsUpdate = false;
   (*solver)(1, data.lines, data.linesUpdate);
	data.linesUpdate = false;
    (*solver)(data.polys, data.polysUpdate);
	data.polysUpdate = false;
  }

  void remove() {}

  ID add(Types type, ObjectData &&data) {
    ID id;
    switch (type) {
    case Types::POINT:
      id.type = 0;
      id.i = this->data.points.size();
      this->data.points.push_back(data);
	  this->data.pointsUpdate = true;
      break;

    case Types::LINE:
      id.type = 1;
      id.i = this->data.lines.size();
      this->data.lines.push_back(data);
	  this->data.linesUpdate = true;
      break;

    default:
      break;
    }

    return id;
  }

  ID add(PolyData &&data) {
    ID id;

    id.type = 2;
    id.i = this->data.polys.size();
    this->data.polys.push_back(data);
	this->data.polysUpdate = true;

    return id;
  }

  std::variant<const ObjectData *, const PolyData *> cget(ID &id) {
    switch (id.type) {
    case 0:
      return &data.points[id.i];
      break;
    case 1:
      return &data.lines[id.i];
      break;

    case 2:
      return &data.polys[id.i];
      break;

    default:
      break;
    }

    return (ObjectData *)nullptr;
  }

  std::variant<ObjectData *, PolyData *> get(ID &id) {
    switch (id.type) {
    case 0:
      data.pointsUpdate = true;
      return &data.points[id.i];
      break;
    case 1:
      data.linesUpdate = true;
      return &data.lines[id.i];
      break;

    case 2:
      data.polysUpdate = true;
      return &data.polys[id.i];
      break;

    default:
      break;
    }

    return (ObjectData *)(nullptr);
  }

  void setSolver(Solver *solver) { this->solver.reset(solver); }

private:
  std::unique_ptr<Solver> solver = nullptr;

  struct __Data {
    std::vector<ObjectData> points;
    std::vector<ObjectData> lines;
    std::vector<PolyData> polys;

    bool pointsUpdate = true;
    bool linesUpdate = true;
    bool polysUpdate = true;
  } data;
};

} // namespace Objects
} // namespace Engine

#endif // OBJECTMANAGER_HPP
