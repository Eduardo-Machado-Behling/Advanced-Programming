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

float pointSide(Vec2 P, Vec2 A, Vec2 B) {
  Vec2 AB = B - A;
  Vec2 AP = P - A;

  return (AB[0] * AP[1]) - (AB[1] * AP[0]);
}

void FindHull(std::vector<Vec2> &hull, std::vector<Vec2> &points, Vec2 P,
              Vec2 Q, uint32_t &recur) {

  recur += 1;
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

  FindHull(hull, S[0], P, C, recur);
  FindHull(hull, S[1], C, Q, recur);
}

std::vector<Vec2> ConvexHull(std::vector<Vec2> points, uint32_t &recursions) {
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

  FindHull(hull, S[0], points.front(), points.back(), recursions);
  FindHull(hull, S[1], points.back(), points.front(), recursions);

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

struct MyWindow : public Engine::Window {
  MyWindow() {
    m_state.addHeader("pointsAmount");
    m_state.addHeader("hullTime");
    m_state.addHeader("recursions");
    m_state.addHeader("hullAmount");
    m_state.addHeader("pointsMeanDist");
    m_state.addHeader("pointsMedianDist");
    m_state.addHeader("pointsStdDist");

    m_state.get("pointsAmount") = 0u;
    m_state.get("hullAmount") = 0u;
    m_state.get("recursions") = 0u;
    m_state.get("hullTime") = (double)0;
    m_state.get("pointsMeanDist") = (double)0;
    m_state.get("pointsStdDist") = (double)0;
  }

  const float POINT_RADIUS = 12;
  const float LINE_STOKE = 2;

  const Line DELAUNAY_COLOR = {0, 0, 1};
  const Line VORONOI_COLOR = {1, 1, 1};
  const Line POINT_COLOR = {1, 0, 0};

  std::vector<Engine::Point> m_points;
  std::vector<Engine::Line> m_lines;
  std::vector<Engine::Poly> m_polys;
  std::vector<Vec2> points;
  bool refresh = true;

  const double timeout = 5;
  void update(double dt) override {
    m_state.get("hullTime") = (double)0;
    if (timer <= timeout) {
      timer += dt;
    } else if (res) {
      res = 0;
      std::cout << "timer: " << res << '\n';
    }

    if (refresh) {
      m_engine->clear();

      std::vector<float> dists;
      for (size_t i = 0; i < points.size(); i++) {
        Vec2 p = points[i];

        for (size_t j = i + 1; j < points.size(); j++) {
          Vec2 b = points[j];

          dists.push_back((b - p).mag());
        }

        m_points.emplace_back(
            std::move(m_engine->createPoint(p, POINT_COLOR, POINT_RADIUS)));
      }

      if (points.size() > 2) {
        float var = 0;
        float mean = 0;
        for (auto dist : dists) {
          mean += dist;
          var += dist * dist;
        }
        mean /= dists.size();
        var /= (dists.size() - 1);
        var = sqrt(var - (mean * mean));

		std::sort(dists.begin(), dists.end());

		size_t medianI = dists.size() / 2;
		double median = dists[medianI];
		if(dists.size() % 2 == 0){
			median += dists[medianI + 1];
			median /= 2;
		}

        m_state.get("pointsMeanDist") = (double)mean;
        m_state.get("pointsMedianDist") = (double)median;
        m_state.get("pointsStdDist") = (double)var;
      }

      if (points.size() > 2) {

        uint32_t recursion = 0;
        auto start = Clock::now();
        auto hull = ConvexHull(points, recursion);
        std::chrono::duration<double> seconds = Clock::now() - start;
        m_state.get("hullTime") = seconds.count();
        m_state.get("recursions") = recursion;
        m_state.get("hullAmount") = (uint32_t)hull.size();

        for (size_t i = 0; i < hull.size(); i++) {
          size_t ni = (i + 1) % hull.size();
          m_lines.emplace_back(std::move(m_engine->createLine(
              hull[i], hull[ni], DELAUNAY_COLOR, LINE_STOKE)));
        }
      }

      std::get<2>(m_state.get("pointsAmount")) = points.size();

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
      std::cout << glx << ", " << gly << ": " << uid << " -> " << type << '\n';

      if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (uid != 0 && m_engine->getType(uid) == 0) {
          return;
        }
        points.push_back({glx, gly});
        refresh = true;
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (uid == 0) {
          return;
        }

        if (m_engine->getType(uid) == 0) {
          std::erase_if(points, [=, this](Vec2 &p) {
            float mag =
                (p[0] - glx) * (p[0] - glx) + (p[1] - gly) * (p[1] - gly);
            return mag <= POINT_RADIUS;
          });
          refresh = true;
        }
      }
    }
  }

  uint32_t res = 0;
  uint32_t scaler = 1;

  Vec2 genRandomPoint() {
    static std::uniform_real_distribution<float> distrib(0.0f, 1.0f);
    return {distrib(gen) * m_windowSize[0], distrib(gen) * m_windowSize[1]};
  }

  void createRandomPoint() {
    static std::uniform_int_distribution<uint32_t> fi(0.0, 100.0);
    uint32_t amount = res;
    if (amount == 0) {
      amount = fi(gen);
    }

    for (size_t i = 0; i < amount; i++) {
      points.push_back(genRandomPoint());
    }
    refresh = true;
  }

  void createTriangle() {
    static std::uniform_int_distribution<uint32_t> fi(0.0, 100.0);
    static std::uniform_int_distribution<uint32_t> id(0, 2);
    static std::uniform_real_distribution<float> fd(0.0, 1.0);

    uint32_t amount = res;
    if (amount == 0) {
      amount = fi(gen);
    }
    Vec2 points[3] = {genRandomPoint(), genRandomPoint(), genRandomPoint()};

    for (size_t i = 0; i < amount; i++) {
      uint32_t start = id(gen), end = id(gen);
      float prog = fd(gen);

      Vec2 A = points[start], B = points[end];

      Vec2 dir = B - A;

      Vec2 p = A + dir * prog;
      this->points.push_back(p);
    }

    refresh = true;
  }

  void createQuad() {
    static std::uniform_int_distribution<uint32_t> fi(0.0, 100.0);
    static std::uniform_int_distribution<uint32_t> id(0, 3);
    static std::uniform_real_distribution<float> fd(0.0, 1.0);

    uint32_t amount = res;
    if (amount == 0) {
      amount = fi(gen);
    }
    Vec2 points[4] = {genRandomPoint(), genRandomPoint(), genRandomPoint(),
                      genRandomPoint()};

    for (size_t i = 0; i < amount; i++) {
      uint32_t start = id(gen), end = id(gen);
      float prog = fd(gen);

      Vec2 A = points[start], B = points[end];

      Vec2 dir = B - A;

      Vec2 p = A + dir * prog;
      this->points.push_back(p);
    }

    refresh = true;
  }

  void createCircle() {
    static std::uniform_int_distribution<uint32_t> fi(0.0, 100.0);
    static std::uniform_real_distribution<float> fd(0.0, 1.0);
    static std::uniform_real_distribution<float> fa(0.0, 2 * 3.1415);

    uint32_t amount = res;
    if (amount == 0) {
      amount = fi(gen);
    }

    Vec2 center = m_mouse;

    center[1] = m_windowSize[1] - center[1];

    float radius = fd(gen) * (m_windowSize - center).min();

    for (size_t i = 0; i < amount; i++) {
      float angle = fa(gen);
      Vec2 dir = {cosf(angle), sinf(angle)};

      Vec2 p = center + dir * radius;
      this->points.push_back(p);
    }

    refresh = true;
  }

  void clear() {
    points.clear();
    refresh = true;
  }

  double timer = 0;

  void keyCallback(int key, int scancode, int action, int mode) override {
    if (action == GLFW_PRESS) {
      bool num = false;
      switch (key) {
      case GLFW_KEY_GRAVE_ACCENT:
        createRandomPoint();
        break;

      case GLFW_KEY_F1:
        createTriangle();
        break;

      case GLFW_KEY_Q:
        clear();
        break;

      case GLFW_KEY_F2:
        createQuad();
        break;

      case GLFW_KEY_F3:
        createCircle();
        break;

      case GLFW_KEY_0:
      case GLFW_KEY_1:
      case GLFW_KEY_2:
      case GLFW_KEY_3:
      case GLFW_KEY_4:
      case GLFW_KEY_5:
      case GLFW_KEY_6:
      case GLFW_KEY_7:
      case GLFW_KEY_8:
      case GLFW_KEY_9:
        res *= 10;
        res += (key - GLFW_KEY_0);
        timer = 0;
        num = true;
        break;

      default:
        break;
      }

      if (!num) {
        res = 0;
      }

      std::cout << "res: " << res << std::endl;
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
