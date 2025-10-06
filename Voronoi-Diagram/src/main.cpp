#include "GLFW/glfw3.h"
#include "Wrappers/Line.hpp"
#include "Wrappers/Point.hpp"
#include "Wrappers/Poly.hpp"
#include "engine.hpp"
#include "window.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <random>
#include <vector>

using Vec2 = Engine::Math::Vector<2>;
using Line = Engine::Math::Vector<3>;
using Cell = std::vector<Vec2>;

const float EPSILON = 1e-18f;

struct DelaunayEdge {
  Vec2 p1;
  Vec2 p2;
};

struct PairHasher {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    return h1 ^ (h2 << 1);
  }
};

inline Line RgbToXyz(const Line &rgb) {
  auto linearize = [](float value) {
    if (value > 0.04045f) {
      return powf((value + 0.055f) / 1.055f, 2.4f);
    }
    return value / 12.92f;
  };

  float r = linearize(rgb[0]);
  float g = linearize(rgb[1]);
  float b = linearize(rgb[2]);

  float x = r * 0.4124f + g * 0.3576f + b * 0.1805f;
  float y = r * 0.2126f + g * 0.7152f + b * 0.0722f;
  float z = r * 0.0193f + g * 0.1192f + b * 0.9505f;

  return {x, y, z};
}

inline Line XyzToLab(const Line &xyz) {
  const float ref_x = 0.95047f;
  const float ref_y = 1.00000f;
  const float ref_z = 1.08883f;

  float x = xyz[0] / ref_x;
  float y = xyz[1] / ref_y;
  float z = xyz[2] / ref_z;

  auto f = [](float t) {
    if (t > 0.008856f) { // (6/29)^3
      return cbrtf(t);
    }
    return (7.787f * t) + (16.0f / 116.0f);
  };

  float fx = f(x);
  float fy = f(y);
  float fz = f(z);

  float l = (116.0f * fy) - 16.0f;
  float a = 500.0f * (fx - fy);
  float b = 200.0f * (fy - fz);

  return {l, a, b};
}

inline float CalculateColorDifference(const Line &rgb1, const Line &rgb2) {
  Line lab1 = XyzToLab(RgbToXyz(rgb1));
  Line lab2 = XyzToLab(RgbToXyz(rgb2));

  float deltaL = lab1[0] - lab2[0];
  float deltaA = lab1[1] - lab2[1];
  float deltaB = lab1[2] - lab2[2];

  return sqrtf(deltaL * deltaL + deltaA * deltaA + deltaB * deltaB);
}

inline Line RandomColor(Line avoid, float diff) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<float> distrib(0.0f, 1.0f);

  Line color = {distrib(gen), distrib(gen), distrib(gen)};
  do {
    color = {distrib(gen), distrib(gen), distrib(gen)};
  } while (CalculateColorDifference(color, avoid) < diff);

  return color;
}

inline bool IsClose(Vec2 a, Vec2 b, float eps = EPSILON) {
  return std::fabs(a[0] - b[0]) <= eps * std::max(1.f, std::fabs(a[0])) &&
         std::fabs(a[1] - b[1]) <= eps * std::max(1.f, std::fabs(a[1]));
}

inline Line TwoPointsBisector(Vec2 a, Vec2 b) {
  Vec2 n{b[0] - a[0], b[1] - a[1]};
  float c = 0.5f * (n[0] * (b[0] + a[0]) + n[1] * (b[1] + a[1]));

  // line: n.x*x + n.y*y = c  →  n.x*x + n.y*y - c = 0
  return {n[0], n[1], -c}; // store Ax + By + C = 0
}

inline bool Intersects(const Line &line, const Vec2 &p, const Vec2 &q,
                       float eps = 1e-8f) {
  float lp = line[0] * p[0] + line[1] * p[1] + line[2];
  float lq = line[0] * q[0] + line[1] * q[1] + line[2];
  return (lp < -eps && lq > eps) || (lp > eps && lq < -eps);
}

inline Vec2 Intersect(const Line &line, const Vec2 &p, const Vec2 &q) {
  // solve parametric intersection of segment pq with line
  float dp = line[0] * p[0] + line[1] * p[1] + line[2];
  float dq = line[0] * q[0] + line[1] * q[1] + line[2];
  float t = dp / (dp - dq);
  return {p[0] + t * (q[0] - p[0]), p[1] + t * (q[1] - p[1])};
}

inline bool IsOnPositiveSide(const Line &line, const Vec2 &p) {
  return (line[0] * p[0] + line[1] * p[1] + line[2]) <= 0.f;
}

void RemoveNearDuplicates(std::vector<Vec2> &poly, float eps = EPSILON) {
  if (poly.size() < 2)
    return;
  std::vector<Vec2> cleaned;
  cleaned.push_back(poly[0]);
  for (size_t i = 1; i < poly.size(); ++i) {
    if (!IsClose(poly[i], cleaned.back(), eps)) {
      cleaned.push_back(poly[i]);
    }
  }
  // Check first and last
  if (cleaned.size() > 1 && IsClose(cleaned.front(), cleaned.back(), eps)) {
    cleaned.pop_back();
  }
  poly = std::move(cleaned);
}

// Sorts and Removes Duplicates
// O(N log(N) + N)
void CleanSites(std::vector<Vec2> &sites) {
  // O(N log(N))
  std::sort(sites.begin(), sites.end(), [](const Vec2 &a, const Vec2 &b) {
    return (a[0] < b[0]) || (a[0] == b[0] && a[1] < b[1]);
  });

  // O(N)
  auto last =
      std::unique(sites.begin(), sites.end(),
                  [](const Vec2 &a, const Vec2 &b) { return IsClose(a, b); });

  // O(1) erasure from the back is just size -= amount;
  sites.erase(last, sites.end());
}

// Brute-Force Intersection of Half-Planes
//
// "Computational Geometry: Algorithms and Applications" Chapter 7 by
//  Prof. Dr. Mark de Berg, Dr. Otfried Cheong, Dr. Marc van Kreveld, Prof. Dr.
//  Mark Overmars
//
// O(N * (N - 1) * (N - 1)) -> O(N³)
std::vector<std::vector<Vec2>> ComputeVoronoi(std::vector<Vec2> &input_sites,
                                              const Vec2 &topleft,
                                              const Vec2 &bottomright,
                                              uint32_t &m) {
  std::vector<Vec2> &sites = input_sites;
  std::vector<Vec2> box = {{topleft[0], topleft[1]},
                           {bottomright[0], topleft[1]},
                           {bottomright[0], bottomright[1]},
                           {topleft[0], bottomright[1]}};

  std::vector<std::vector<Vec2>> cells;
  cells.reserve(sites.size());

  m = 0;
  // O(N)
  for (size_t i = 0; i < sites.size(); ++i) {
    std::vector<Vec2> cell = box;

    // O(N - 1)
    for (size_t j = 0; j < sites.size(); ++j) {
      // -1 from O(N - 1)
      if (i == j)
        continue;

      Line bisector = TwoPointsBisector(sites[i], sites[j]);
      if (std::isnan(bisector[2]))
        continue; // skip degenerate

      std::vector<Vec2> new_cell;
      new_cell.reserve(cell.size());

      // O(M) where M = vertices in polygon can be at worst N - 1
      // so O(N - 1)
      m += cell.size();
      for (size_t k = 0; k < cell.size(); ++k) {
        size_t nk = (k + 1) % cell.size();
        const Vec2 &p = cell[k];
        const Vec2 &q = cell[nk];

        bool ppos = IsOnPositiveSide(bisector, p);
        bool qpos = IsOnPositiveSide(bisector, q);

        if (ppos)
          new_cell.push_back(p);
        if (ppos != qpos) {
          new_cell.push_back(Intersect(bisector, p, q));
        }
      }

      cell.swap(new_cell);

      // Not a polygon
      if (cell.size() < 3)
        break;
    }

    cells.push_back(std::move(cell)); // may be empty
  }

  return cells;
}

// "Computational Geometry: Algorithms and Applications" Chapter 9 by
//  Prof. Dr. Mark de Berg, Dr. Otfried Cheong, Dr. Marc van Kreveld, Prof. Dr.
//  Mark Overmars
//
// O(N² + N) -> O(N²)
std::vector<DelaunayEdge>
ComputeDelaunay(const std::vector<Vec2> &sites,
                const std::vector<std::vector<Vec2>> &voronoi_cells) {

  std::vector<DelaunayEdge> delaunay_edges;

  std::vector<Vec2> unique_vertices;
  std::map<std::pair<size_t, size_t>, int> vertex_map;

  // O(N) where N = voronoi vertices
  for (size_t i = 0; i < voronoi_cells.size(); ++i) {
    for (size_t j = 0; j < voronoi_cells[i].size(); ++j) {
      const Vec2 &v = voronoi_cells[i][j];
      int unified_idx = -1;

      // O(N) Linear search
      for (size_t k = 0; k < unique_vertices.size(); ++k) {
        if (IsClose(v, unique_vertices[k], 1e-4f)) { // Use a larger tolerance
          unified_idx = k;
          break;
        }
      }

      // If no close vertex was found, add this one as a new unique vertex
      if (unified_idx == -1) {
        unified_idx = unique_vertices.size();
        unique_vertices.push_back(v);
      }
      vertex_map[{i, j}] = unified_idx;
    }
  }

  // Finding shared edges using integer indices
  // Key is a pair of unified vertex indices, value is the site index
  std::unordered_map<std::pair<size_t, size_t>, size_t, PairHasher>
      edge_to_site_map;

  // O(N) where N = voronoi vertices
  for (size_t i = 0; i < voronoi_cells.size(); ++i) {
    const auto &cell = voronoi_cells[i];
    if (cell.size() < 2)
      continue;
    for (size_t j = 0; j < cell.size(); ++j) {
      int idx1 = vertex_map.at({i, j});
      int idx2 = vertex_map.at({i, (j + 1) % cell.size()});

      std::pair<int, int> edge_key = {std::min(idx1, idx2),
                                      std::max(idx1, idx2)};

      if (edge_to_site_map.count(edge_key)) {
        size_t other_site_idx = edge_to_site_map[edge_key];
        delaunay_edges.push_back({sites[i], sites[other_site_idx]});
        edge_to_site_map.erase(edge_key);
      } else {
        edge_to_site_map[edge_key] = i;
      }
    }
  }

  return delaunay_edges;
}

struct MyWindow : public Engine::Window {
  MyWindow()
      : points({
            {{200, 200}, DELAUNAY_COLOR},
            {{800, 800}, DELAUNAY_COLOR},
            {{500, 920}, DELAUNAY_COLOR},
        }) {
    m_state.addHeader("pointsAmount");
    m_state.addHeader("voronoiTime");
    m_state.addHeader("delaunayTime");
    m_state.addHeader("M");

    m_state.get("pointsAmount") = 0u;
    m_state.get("M") = 0u;
    m_state.get("voronoiTime") = (double)0;
    m_state.get("delaunayTime") = (double)0;
  }

  struct Point {
    Vec2 pos;
    Line color;

    Point(Vec2 pos, Line avoid) : pos(pos), color(RandomColor(avoid, 20)) {}
  };

  const float POINT_RADIUS = 12;
  const float LINE_STOKE = 2;

  const Line DELAUNAY_COLOR = {0, 0, 1};
  const Line VORONOI_COLOR = {1, 1, 1};
  const Line POINT_COLOR = {1, 0, 0};

  std::vector<Engine::Point> m_points;
  std::vector<Engine::Line> m_lines;
  std::vector<Engine::Poly> m_polys;
  std::vector<Point> points;
  bool refresh = true;

  void update(double dt) override {
    m_state.get("voronoiTime") = (double)0;
    m_state.get("delaunayTime") = (double)0;

    if (refresh) {
      m_engine->clear();

      std::vector<Vec2> sites;
      sites.reserve(points.size());
      for (auto &p : points) {
        m_points.emplace_back(
            std::move(m_engine->createPoint(p.pos, POINT_COLOR, POINT_RADIUS)));
        sites.push_back(p.pos);
      }

      std::sort(points.begin(), points.end(),
                [](const Point &a, const Point &b) {
                  return (a.pos[0] < b.pos[0]) ||
                         (a.pos[0] == b.pos[0] && a.pos[1] < b.pos[1]);
                });
      CleanSites(sites);

      std::get<2>(m_state.get("pointsAmount")) = sites.size();
      uint32_t m = 0;
      Clock::time_point start = Clock::now();
      auto my_diagram = ComputeVoronoi(
          sites, {0, 0},
          {(float)m_engine->winSize()[0], (float)m_engine->winSize()[1]}, m);

      Clock::time_point end = Clock::now();
      m_state.get("M") = m;
      double dur = std::chrono::duration<double>(end - start).count();
      m_state.get("voronoiTime") = dur;

      start = Clock::now();
      auto delaunay = ComputeDelaunay(sites, my_diagram);
      end = Clock::now();
      dur = std::chrono::duration<double>(end - start).count();
      m_state.get("delaunayTime") = dur;

      for (auto &edge : delaunay) {
        m_lines.emplace_back(std::move(m_engine->createLine(
            edge.p1, edge.p2, DELAUNAY_COLOR, LINE_STOKE)));
      }

      for (size_t i = 0; i < my_diagram.size(); i++) {
        auto &cell = my_diagram[i];
        auto &p = points[i];
        for (size_t j = 0; j < cell.size(); j++) {
          auto &vert = cell[j];
          size_t nj = (j + 1) % cell.size();

          m_lines.emplace_back(std::move(
              m_engine->createLine(vert, cell[nj], VORONOI_COLOR, LINE_STOKE)));
        }
        m_polys.emplace_back(
            std::move(m_engine->createPoly(cell, p.color, p.color, 0)));
      }

      refresh = false;
    }
  }

private:
  void mouseButtonCallback(int button, int action, int mods) override {
    if (action == GLFW_PRESS) {
      double x, y;
      glfwGetCursorPos(m_window, &x, &y);

      int width, height;
      glfwGetWindowSize(m_window, &width, &height);

      float glx = (float)x;
      float gly = height - y;

      auto uid = m_engine->lookupObjectUUID(glx, gly);
      int type = m_engine->getType(uid);
      std::cout << glx << ", " << gly << ": " << uid << " -> " << type << '\n';

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (uid != 0 && m_engine->getType(uid) == 0) {
          return;
        }
        points.push_back({{glx, gly}, DELAUNAY_COLOR});
        refresh = true;
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (uid == 0) {
          return;
        }

        if (m_engine->getType(uid) == 0) {
          std::erase_if(points, [=, this](Point &point) {
            Vec2 p = point.pos;
            float mag =
                (p[0] - glx) * (p[0] - glx) + (p[1] - gly) * (p[1] - gly);
            return mag <= POINT_RADIUS;
          });
          refresh = true;
        }
      }
    }
  }

  void keyCallback(int key, int scancode, int action, int mode) override {}
};

int main() {
  MyWindow win;
  while (win.isActivate()) {
    win.gameloop();
  }

  return 0;
}
