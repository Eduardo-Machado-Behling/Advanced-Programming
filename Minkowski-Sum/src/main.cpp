#include "GLFW/glfw3.h"
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
using Color = Engine::Math::Vector<3>;
using Cell = std::vector<Vec2>;
using Polygon = std::vector<Vec2>;

float pointSide(Vec2 P, Vec2 A, Vec2 B) {
  Vec2 AB = B - A;
  Vec2 AP = P - A;

  return (AB[0] * AP[1]) - (AB[1] * AP[0]);
}

void FindHull(std::vector<Vec2> &hull, std::vector<Vec2> &points, Vec2 P,
              Vec2 Q) {

  if (points.empty()) {
    return;
  }

  struct {
    float dist = 0;
    size_t i = 0;
  } max;

  Vec2 delta = Q - P;
  Vec2 dir = delta;
  dir.norm();
  for (size_t i = 0; i < points.size(); i++) {
    float dist = std::abs(pointSide(points[i], P, Q));
    if (max.dist < dist) {
      max.dist = dist;
      max.i = i;
    }
  }

  Vec2 C = points[max.i];
  auto it = std::find(hull.begin(), hull.end(), P);
  if (it == hull.end()) {
    throw std::runtime_error("P not find in hull");
  }
  hull.emplace(++it, C);

  std::vector<Vec2> S[2];

  for (size_t i = 0; i < points.size(); i++) {
    if (i == max.i)
      continue;

    float sidePC = pointSide(points[i], P, C);
    float sideCQ = pointSide(points[i], C, Q);

    if (sidePC > 0) {
      S[0].push_back(points[i]);
    }

    if (sideCQ > 0) {
      S[1].push_back(points[i]);
    }
  }

  FindHull(hull, S[0], P, C);
  FindHull(hull, S[1], C, Q);
}

std::vector<Vec2> ConvexHull(std::vector<Vec2> points) {
  std::vector<Vec2> hull;

  std::sort(points.begin(), points.end(),
            [](Vec2 a, Vec2 b) { return a[0] < b[0]; });

  size_t left = 0;
  size_t right = points.size();

  hull.push_back(points.front());
  hull.push_back(points.back());

  std::vector<Vec2> S[2];

  float midx = (points.front()[0] + points.back()[0]) / 2;
  size_t mid = 0;

  for (size_t i = 1; i < points.size() - 1 && mid == 0; i++) {
    float side = pointSide(points[i], points.front(), points.back());
    S[side < 0].push_back(points[i]);
  }

  FindHull(hull, S[0], points.front(), points.back());
  FindHull(hull, S[1], points.back(), points.front());

  Vec2 centroid = {0.0f, 0.0f};
  for (const auto &p : hull) {
    centroid[0] += p[0];
    centroid[1] += p[1];
  }
  centroid[0] /= hull.size();
  centroid[1] /= hull.size();

  std::sort(hull.begin(), hull.end(), [&](const Vec2 &a, const Vec2 &b) {
    float angleA = atan2(a[1] - centroid[1], a[0] - centroid[0]);
    float angleB = atan2(b[1] - centroid[1], b[0] - centroid[0]);
    return angleA < angleB;
  });

  return hull;
}

// Helper function to find the 2D cross product of vectors AB and AC
// This tells us if C is to the left or right of the line AB
// > 0: C is to the left
// < 0: C is to the right
// = 0: C is collinear
float cross_product(Vec2 a, Vec2 b) { return a[0] * b[1] - a[1] * b[0]; }

// Helper function to find the index of the vertex with the minimum Y-coordinate
// This is a robust way to find a "starting" vertex on the convex hull.
size_t findMinYVertex(const Polygon &poly) {
  size_t minIdx = 0;
  for (size_t i = 1; i < poly.size(); ++i) {
    if (poly[i][1] < poly[minIdx][1] ||
        (poly[i][1] == poly[minIdx][1] && poly[i][0] < poly[minIdx][0])) {
      minIdx = i;
    }
  }
  return minIdx;
}

Polygon MinkowskiSum_impl(const Polygon &A, const Polygon &B) {
  size_t m = A.size();
  size_t n = B.size();

  if (m == 0)
    return B;
  if (n == 0)
    return A;

  size_t idxA = findMinYVertex(A);
  size_t idxB = findMinYVertex(B);

  Polygon C;
  C.reserve(m + n);

  C.push_back(A[idxA] + B[idxB]);

  size_t i = 0; // Edge counter for A
  size_t j = 0; // Edge counter for B
  size_t currA = idxA;
  size_t currB = idxB;

  while (i < m || j < n) {
    if (i == m) {
      Vec2 edgeB = B[(currB + 1) % n] - B[currB % n];
      C.push_back(C.back() + edgeB);
      currB = (currB + 1) % n;
      j++;
    }

    else if (j == n) {
      Vec2 edgeA = A[(currA + 1) % m] - A[currA % m];
      C.push_back(C.back() + edgeA);
      currA = (currA + 1) % m;
      i++;
    }

    // compare the angles of the next edges
    else {
      Vec2 edgeA = A[(currA + 1) % m] - A[currA % m];
      Vec2 edgeB = B[(currB + 1) % n] - B[currB % n];

      float cp = cross_product(edgeA, edgeB);

      // If cp >= 0, edgeA has a smaller (or equal) angle than edgeB.
      if (cp >= 0) {
        C.push_back(C.back() + edgeA);
        currA = (currA + 1) % m;
        i++;
      } else {
        C.push_back(C.back() + edgeB);
        currB = (currB + 1) % n;
        j++;
      }
    }
  }

  // Remove the last vertex, which is a duplicate of the first
  C.pop_back();

  return C;
}

Vec2 getCentroid(const Polygon &poly) {
  Vec2 centroid = {0.0f, 0.0f};
  if (poly.empty())
    return centroid;
  for (const auto &p : poly) {
    centroid[0] += p[0];
    centroid[1] += p[1];
  }
  centroid[0] /= poly.size();
  centroid[1] /= poly.size();
  return centroid;
}

Polygon createOrigin(const Polygon &poly, bool reverse = false) {
  Vec2 centroid = getCentroid(poly);

  Polygon reflected_poly;
  reflected_poly.reserve(poly.size());
  for (const auto &vertex : poly) {
    reflected_poly.push_back((vertex - centroid) * (reverse ? -1 : 1));
  }
  return reflected_poly;
}

std::vector<Polygon> MinkowskiSum(Polygon &robot, std::vector<Polygon> &obs) {
  std::vector<Polygon> res;
  Polygon rrobot = createOrigin(robot, true);

  for (size_t i = 0; i < obs.size(); i++) {
    Polygon obj = obs[i];
    Vec2 center = getCentroid(obj);

    Polygon sum = MinkowskiSum_impl(createOrigin(obj), rrobot);

    for (auto &vec : sum) {
      vec += center;
    }

    res.push_back(sum);
  }

  return res;
}

void dumpDistances(const Polygon &robot, const std::vector<Polygon> &obs) {
  std::ofstream dist("dist.csv");
  dist << "obj (#),robotVert,sumVert,minDist\n";
  for (size_t i = 0; i < obs.size(); i++) {
    dist << i << ',';
    struct {
      float dist = INFINITY;
      size_t robotidx = 0;
      size_t obsidx = 0;
    } min;

    for (size_t j = 0; j < robot.size(); j++) {
      for (size_t k = 0; k < obs[i].size(); k++) {
        float dist = (obs[i][k] - robot[j]).mag();
        if (dist < min.dist) {
          min.dist = dist;
          min.robotidx = j;
          min.obsidx = k;
        }
      }
    }

    dist << "\"(" << robot[min.robotidx][0] << ',' << robot[min.robotidx][1]
         << ")\",";
    dist << "\"(" << obs[i][min.obsidx][0] << ',' << obs[i][min.obsidx][1]
         << ")\",";
    dist << min.dist << '\n';
  }
}

struct MyWindow : public Engine::Window {
  MyWindow() {
    m_state.addHeader("objPoints");
    m_state.addHeader("robotPoints");
    m_state.addHeader("sumTime");

    m_state.get("objPoints") = 0u;
    m_state.get("robotPoints") = 0u;
    m_state.get("sumTime") = (double)0;
  }

  const float POINT_RADIUS = 12;
  const float LINE_STOKE = 2;

  const Color ROBOT_COLOR = {0, 0, 1};
  const Color OBS_COLOR = {1, 0, 0};
  const Color LINE_COLOR = {1, 1, 1};
  const Color POINT_ROBOT_COLOR = {0, 1, 1};
  const Color POINT_OBS_COLOR = {1, 1, 0};
  const Color LINE_OUT_COLOR = {0.1, 0.1, 0.1};
  const Color POLY_OUT_COLOR = {0.3, 0.3, 0.3};

  Polygon robot;
  std::vector<Polygon> obs;
  int currObs = -1;
  bool refresh = true;

  const double timeout = 5;
  void update(double dt) override {
    m_state.get("sumTime") = (double)0;

    if (refresh) {
      clearEngine();

      Polygon robotHull;
      if (robot.size() > 2) {
        robotHull = ConvexHull(robot);
        for (size_t i = 0; i < robot.size(); i++)
          m_points.emplace_back(std::move(m_engine->createPoint(
              robot[i], POINT_ROBOT_COLOR, POINT_RADIUS)));
        for (size_t i = 0; i < robotHull.size(); i++) {
          size_t ni = (i + 1) % robotHull.size();

          m_lines.emplace_back(std::move(m_engine->createLine(
              robotHull[i], robotHull[ni], LINE_COLOR, LINE_STOKE)));
        }
        m_polys.emplace_back(std::move(
            m_engine->createPoly(robotHull, ROBOT_COLOR, ROBOT_COLOR, 0)));
      } else {
        for (size_t i = 0; i < robot.size(); i++) {
          size_t ni = (i + 1) % robot.size();

          m_points.emplace_back(std::move(m_engine->createPoint(
              robot[i], POINT_ROBOT_COLOR, POINT_RADIUS)));
          m_lines.emplace_back(std::move(m_engine->createLine(
              robot[i], robot[ni], LINE_COLOR, LINE_STOKE)));
        }
      }

      uint32_t points = 0;
      std::vector<Polygon> obsHull;
      for (size_t i = 0; i < obs.size(); i++) {
        if (obs[i].size() > 2) {
          obsHull.push_back(ConvexHull(obs[i]));
          for (size_t j = 0; j < obs[i].size(); j++) {
            m_points.emplace_back(std::move(m_engine->createPoint(
                obs[i][j], POINT_OBS_COLOR, POINT_RADIUS)));
          }
          points += obsHull.back().size();
        } else {
          for (size_t j = 0; j < obs[i].size(); j++) {
            size_t nj = (j + 1) % obs[i].size();
            m_points.emplace_back(std::move(m_engine->createPoint(
                obs[i][j], POINT_OBS_COLOR, POINT_RADIUS)));
            if (obs[i].size() > 1)
              m_lines.emplace_back(std::move(m_engine->createLine(
                  obs[i][j], obs[i][nj], LINE_COLOR, LINE_STOKE)));
          }
        }
      }
      m_state.get("objPoints") = points;
      m_state.get("robotPoints") = (uint32_t)robotHull.size();

      if (!robotHull.empty() && !obsHull.empty() && currObs == -1) {

        auto start = Clock::now();
        std::vector<Polygon> sum = MinkowskiSum(robotHull, obsHull);
        std::chrono::duration<double> seconds = Clock::now() - start;
        m_state.get("sumTime") = seconds.count();

        for (size_t i = 0; i < sum.size(); i++) {
          for (size_t j = 0; j < sum[i].size(); j++) {
            size_t nj = (j + 1) % sum[i].size();

            m_points.emplace_back(std::move(m_engine->createPoint(
                sum[i][j], POINT_OBS_COLOR, POINT_RADIUS)));
            m_lines.emplace_back(std::move(m_engine->createLine(
                sum[i][j], sum[i][nj], LINE_OUT_COLOR, LINE_STOKE)));
          }
          m_polys.emplace_back(std::move(
              m_engine->createPoly(sum[i], POLY_OUT_COLOR, POLY_OUT_COLOR, 0)));
        }

        dumpDistances(robotHull, sum);
      }

      for (size_t i = 0; i < obsHull.size(); i++) {
        for (size_t j = 0; j < obsHull[i].size(); j++) {
          size_t nj = (j + 1) % obsHull[i].size();

          m_lines.emplace_back(std::move(m_engine->createLine(
              obsHull[i][j], obsHull[i][nj], LINE_COLOR, LINE_STOKE)));
        }

        m_polys.emplace_back(std::move(
            m_engine->createPoly(obsHull[i], OBS_COLOR, OBS_COLOR, 0)));
      }

      refresh = false;
    }
  }

private:
  void mouseButtonCallback(int button, int action, int mods) override {
    if (action == GLFW_PRESS) {
      float glx = m_mouse[0];
      float gly = m_windowSize[1] - m_mouse[1];

      auto uid = m_engine->lookupObjectUUID(glx, gly);
      int type = m_engine->getType(uid);

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (uid != 0 && m_engine->getType(uid) == 0) {
          return;
        }
        if (currObs != -1) {
          obs[currObs].push_back({glx, gly});
        } else {
          robot.push_back({glx, gly});
        }
        refresh = true;
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (uid == 0) {
          return;
        }
      }
    }
  }

  Vec2 genRandomPoint() {
    static std::uniform_real_distribution<float> distrib(0.0f, 1.0f);
    return {distrib(gen) * m_windowSize[0], distrib(gen) * m_windowSize[1]};
  }

  void genRandom() {
    static std::uniform_real_distribution<float> df(-100.f, 100.f);
    static std::uniform_int_distribution<uint32_t> di(4, 200);

    uint32_t amount = di(gen);
    Polygon &p = obs.emplace_back();
    p.reserve(amount);

    for (size_t i = 0; i < amount; i++) {
      p.push_back({df(gen), df(gen)});
      p.back() += {m_mouse[0], m_windowSize[1] - m_mouse[1]};
    }

    refresh = true;
  }

  void clear() {
    robot.clear();
    obs.clear();
    refresh = true;
  }

  void keyCallback(int key, int scancode, int action, int mode) override {
    if (action == GLFW_PRESS) {
      bool num = false;
      switch (key) {
      case GLFW_KEY_Q:
        clear();
        break;

      case GLFW_KEY_ESCAPE:
        terminate();
        break;

      case GLFW_KEY_F1:
        genRandom();
        break;

      case GLFW_KEY_LEFT_SHIFT:
        currObs = obs.size();
        obs.emplace_back();
        break;

      default:
        break;
      }

    } else if (action == GLFW_RELEASE) {
      switch (key) {
      case GLFW_KEY_LEFT_SHIFT:
        if (obs[currObs].size() < 3) {
          obs.erase(obs.begin() + currObs);
        }
        currObs = -1;
        refresh = true;
        break;

      default:
        break;
      }
    }
  }
};

int main() {
  MyWindow win;
  while (win.isActivate()) {
    win.gameloop();
  }

  return 0;
}
