#include "Path/Manager.hpp"
#include "Grid/Manager.hpp"
#include "Path/Dijkstra.hpp"
#include <iostream>

#include "Tracy.hpp"

struct PathManager::PathID {

  std::list<std::vector<Vec2u>>::iterator dataRef;

  DynamicInfo info;
  Vec2u start;
  Vec2u end;

  OnChangeCallback callback = nullptr;

  PathID(decltype(dataRef) ref, DynamicInfo i, Vec2u s, Vec2u e)
      : dataRef(ref), info(i), start(s), end(e) {}
};

void PathManager::PathDeleter::operator()(PathID *p) const {
  if (p) {
    get().unregisterPath(p);
  }

  delete p;
}

PathManager &PathManager::get() {
  static PathManager *m_instance = new PathManager();
  return *m_instance;
}

PathManager::PathManager() {

  auto onChange = [this]() {
    std::cout << "[PathManager] Grid changed. Invalidating paths...\n";
    this->invalidateAll();
  };
  m_onGridChange.setOnChange(onChange);
  GridManager::get().subscribeOnGridChange(&m_onGridChange);
}

PathManager::~PathManager() {

  m_registry.clear();
  m_pathStore.clear();
}

PathManager::PathPtr PathManager::requestPath(DynamicInfo info, Vec2u start,
                                              Vec2u end) {
  ZoneScoped;

  m_pathStore.emplace_front();
  auto it = m_pathStore.begin();

  PathID *id = new PathID(it, info, start, end);
  *id->dataRef =
      Dijkstra::ShortestPath(id->start, id->end, GridManager::get().getGraph());

  m_registry.insert(id);

  return PathPtr(id);
}

void PathManager::registerOnChange(PathID *id, OnChangeCallback callback) {
  id->callback = callback;
}

void PathManager::unregisterPath(PathID *id) {
  ZoneScoped;
  if (!id)
    return;

  m_registry.erase(id);

  m_pathStore.erase(id->dataRef);
}

void PathManager::invalidateAll() {

  for (auto *id : m_registry) {
    id->dataRef->clear();
  }
}

void PathManager::update() {
  ZoneScoped;

  for (auto *id : m_registry) {

    std::vector<Vec2u> newPath = Dijkstra::ShortestPath(
        id->start, id->end, GridManager::get().getGraph());

    if (newPath.empty()) {

    } else {
      *id->dataRef = std::move(newPath);
    }

    if (id->callback) {
      id->callback();
    }
  }
}

std::optional<Vec2> PathManager::getSegment(PathID *id, size_t i) {
  if (!id || i >= id->dataRef->size())
    return std::nullopt;

  return GridManager::get().getCenter((*id->dataRef)[i]);
}