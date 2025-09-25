#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Objects/ObjectData.hpp"
#include "Objects/ObjectUUID.hpp"

namespace Engine {
namespace Objects {

class ObjectManager {
public:
  struct Solver {
    virtual void operator()(size_t i, std::vector<ObjectData> &data,
                            bool update) = 0;
    virtual void operator()(std::vector<PolyData> &data, bool update) = 0;

    virtual uint32_t getDrawCalls() = 0;
  };

  struct ObjectCount {
    uint32_t points = 0;
    uint32_t lines = 0;
    uint32_t polys = 0;
  };

  enum class Types {
    POINT,
    LINE,
    POLY,
  };

  void draw() {
    (*solver)(0, data.points, data.update[0]);
    data.update[0] = false;
    (*solver)(1, data.lines, data.update[1]);
    data.update[1] = false;
    (*solver)(data.polys, data.update[2]);
    data.update[2] = false;
    m_drawCalls = solver->getDrawCalls();
  }

  uint32_t drawCalls() { return m_drawCalls; }
  uint32_t entities() { return data.uuid2ii.size(); }
  ObjectCount count() { return m_count; }

  int type(ObjectUUID::UUID id) { return data.uuid2ii[id].first; }

  void remove(ObjectUUID::UUID id) {
    std::pair<uint32_t, uint32_t> ii = data.uuid2ii.at(id);

    std::pair<uint32_t, uint32_t> iiTemp[2];
    switch (ii.first) {
    case 0:
      m_count.points -= 1;
      iiTemp[0].first = 0;
      iiTemp[1].first = 0;
      for (size_t i = ii.second + 1; i < data.points.size(); i++) {
        iiTemp[0].second = i;
        iiTemp[1].second = i - 1;

        uint32_t uuidOld = data.ii2uuid.at(iiTemp[0]);
        data.ii2uuid.erase(iiTemp[0]);

        data.uuid2ii[uuidOld] = iiTemp[1];
        data.ii2uuid.emplace(iiTemp[1], uuidOld);
      }

      data.points.erase(std::next(data.points.begin(), ii.second));
      break;

    case 1:
      m_count.lines -= 1;
      iiTemp[0].first = 1;
      iiTemp[1].first = 1;
      for (size_t i = ii.second + 1; i < data.lines.size(); i++) {
        iiTemp[0].second = i;
        iiTemp[1].second = i - 1;

        uint32_t uuidOld = data.ii2uuid.at(iiTemp[0]);
        data.ii2uuid.erase(iiTemp[0]);

        data.uuid2ii[uuidOld] = iiTemp[1];
        data.ii2uuid.emplace(iiTemp[1], uuidOld);
      }

      data.lines.erase(std::next(data.lines.begin(), ii.second));
      break;
    case 2:
      m_count.polys -= 1;
      iiTemp[0].first = 2;
      iiTemp[1].first = 2;
      for (size_t i = ii.second + 1; i < data.polys.size(); i++) {
        iiTemp[0].second = i;
        iiTemp[1].second = i - 1;

        uint32_t uuidOld = data.ii2uuid.at(iiTemp[0]);
        data.ii2uuid.erase(iiTemp[0]);

        data.uuid2ii[uuidOld] = iiTemp[1];
        data.ii2uuid.emplace(iiTemp[1], uuidOld);
      }

      data.polys.erase(std::next(data.polys.begin(), ii.second));
      break;
    }

    data.ii2uuid.erase(ii);
    data.uuid2ii.erase(id);
    uuid.remove(id);
  }

  ObjectUUID::UUID add(Types type, ObjectData &&data) {
    switch (type) {
    case Types::POINT: {
      m_count.points += 1;
      ObjectUUID::UUID id = this->uuid.get();
      data.uuid = id;

      size_t type = 0;
      size_t i = this->data.points.size();
      this->data.points.push_back(data);
      this->data.update[type] = true;

      std::pair<uint32_t, uint32_t> ii = std::make_pair(type, i);
      this->data.uuid2ii.emplace(id, ii);
      this->data.ii2uuid.emplace(ii, id);

      return id;
    } break;

    case Types::LINE: {
      m_count.lines += 1;
      ObjectUUID::UUID id = this->uuid.get();
      data.uuid = id;

      size_t type = 1;
      size_t i = this->data.lines.size();
      this->data.lines.push_back(data);
      this->data.update[type] = true;

      std::pair<uint32_t, uint32_t> ii = std::make_pair(type, i);
      this->data.uuid2ii.emplace(id, ii);
      this->data.ii2uuid.emplace(ii, id);

      return id;
    } break;

    default:
      break;
    }

    throw std::runtime_error("Incorrect type");
  }

  ObjectUUID::UUID add(PolyData &&data) {
    m_count.polys += 1;
    ObjectUUID::UUID id = this->uuid.get();
    data.uuid = id;

    size_t type = 2;
    size_t i = this->data.polys.size();
    this->data.polys.push_back(data);
    this->data.update[type] = true;

    std::pair<uint32_t, uint32_t> ii = std::make_pair(type, i);
    this->data.uuid2ii.emplace(id, ii);
    this->data.ii2uuid.emplace(ii, id);

    return id;
  }

  std::variant<const PolyData *, const ObjectData *>
  cget(ObjectUUID::UUID &id) {
    if (data.uuid2ii.contains(id)) {
      auto &ii = data.uuid2ii.at(id);

      switch (ii.first) {
      case 0:
        return &data.points.at(ii.second);
      case 1:
        return &data.lines.at(ii.second);
      case 2:
        return &data.polys.at(ii.second);
      }
    }
    return (ObjectData *)nullptr;
  }

  std::variant<PolyData *, ObjectData *> get(ObjectUUID::UUID &id) {
    if (data.uuid2ii.contains(id)) {
      auto &ii = data.uuid2ii.at(id);
      data.update[ii.first] = true;

      switch (ii.first) {
      case 0:
        return &data.points.at(ii.second);
      case 1:
        return &data.lines.at(ii.second);
      case 2:
        return &data.polys.at(ii.second);
      }
    }
    return (ObjectData *)nullptr;
  }

  void setSolver(Solver *solver) { this->solver.reset(solver); }

private:
  std::unique_ptr<Solver> solver = nullptr;
  ObjectUUID uuid;

  struct IIHash {
    std::size_t operator()(const std::pair<uint32_t, uint32_t> &id) const {
      // A common way to combine two hashes
      std::size_t hash1 = std::hash<uint32_t>{}(id.first);
      std::size_t hash2 = std::hash<uint32_t>{}(id.second);
      // Combine them using a bit shift and XOR
      return hash1 ^ (hash2 << 1);
    }
  };

  struct __Data {
    std::vector<ObjectData> points;
    std::vector<ObjectData> lines;
    std::vector<PolyData> polys;

    std::unordered_map<ObjectUUID::UUID, std::pair<uint32_t, uint32_t>> uuid2ii;
    std::unordered_map<std::pair<uint32_t, uint32_t>, ObjectUUID::UUID, IIHash>
        ii2uuid;

    bool update[3] = {true, true, true};
  } data;

  uint32_t m_drawCalls;
  ObjectCount m_count;
};
} // namespace Objects
} // namespace Engine

#endif // OBJECTMANAGER_HPP
