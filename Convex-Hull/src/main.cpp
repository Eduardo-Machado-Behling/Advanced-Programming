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

void FindHull(std::vector<Vec2> &hull, std::vector<Vec2> &points, size_t P,
              size_t Q) {
  if ((Q - P) < 2) {
    return;
  }

  struct {
    float dist = 0;
    size_t i = 0;
  } max;

  Vec2 vecQ = points[Q - 1];
  Vec2 vecP = points[P];

  Vec2 delta = vecQ - vecP;
  Vec2 dir = delta;
  dir.norm();
  for (size_t i = P + 1; i < Q - 1; i++) {
    float param = dir.dot(delta) / dir.dot(dir);
    Vec2 dist = vecP + (dir * param);

    float sqrDist = dist.dot(dist);
    if (max.dist < sqrDist) {
      max.dist = sqrDist;
      max.i = i;
    }
  }

  hull.emplace(std::next(hull.rbegin()).base(), points[max.i]);

  Vec2 deltaPC = points[max.i] - vecP;
  Vec2 deltaCQ = vecQ - points[max.i];

  std::vector<Vec2> S[2];
  S[0].push_back(vecP);
  S[1].push_back(points[max.i]);

  for (size_t i = P + 1; i < Q - 1; i++) {
    Vec2 &point = points[i];

    Vec2 deltaP = point - vecP;
    Vec2 deltaC = point - points[max.i];

    float dotP = deltaPC.dot(deltaP);
    float dotC = deltaCQ.dot(deltaC);

    if (dotP > 0) {
      S[0].push_back(point);
    }
    if (dotC > 0) {
      S[1].push_back(point);
    }
  }
  S[0].push_back(points[max.i]);
  S[1].push_back(vecQ);

  FindHull(hull, S[0], 0, S[0].size());
  FindHull(hull, S[1], 0, S[1].size());
}

std::vector<Vec2> ConvexHull(std::vector<Vec2> points) {
  std::vector<Vec2> hull;

  std::sort(points.begin(), points.end(),
            [](Vec2 a, Vec2 b) { return a[0] < b[0]; });

  size_t left = 0;
  size_t right = points.size();

  hull.push_back(points.front());
  hull.push_back(points.back());

  float midx = (points[left][0] + points[right - 1][0]) / 2;
  size_t mid = 0;

  for (size_t i = 1; i < points.size() - 1 && mid == 0; i++) {
    if (points[i][0] > midx) {
      mid = i;
    }
  }

  FindHull(hull, points, left, mid);
  FindHull(hull, points, mid, right);

  return hull;
}

struct MyWindow : public Engine::Window {
  MyWindow() {
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
  std::vector<Vec2> points;
  bool refresh = true;

  void update(double dt) override {
    m_state.get("voronoiTime") = (double)0;
    m_state.get("delaunayTime") = (double)0;

    if (refresh) {
      m_engine->clear();

      for (auto &p : points) {
        m_points.emplace_back(
            std::move(m_engine->createPoint(p, POINT_COLOR, POINT_RADIUS)));
      }

      if (points.size() > 2){
		  auto hull = ConvexHull(points);

		  for (auto &h : hull) {
			  std::cout << '\t' << h[0] << ", " << h[1] << '\n';
		  }
      }


      std::get<2>(m_state.get("pointsAmount")) = points.size();

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

  void keyCallback(int key, int scancode, int action, int mode) override {}
};

int main() {
  MyWindow win;
  while (win.isActivate()) {
    win.gameloop();
  }

  return 0;
}
