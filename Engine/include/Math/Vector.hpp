#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iostream>

namespace Engine {
namespace Math {

template <size_t D, typename S = float> class Vector {
public:
  Vector(std::initializer_list<S> data) {
    auto it = data.begin();
    for (size_t i = 0; i < D; i++) {
      m_data[i] = *it;
      it += 1;
    }
  }

  Vector(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] = scalar;
    }
  }

  Vector() {}

  size_t size() { return D * sizeof(S); }

  bool operator==(Vector<D, S> &vec) {
    for (size_t i = 0; i < D; i++) {
      if (m_data[i] != vec[i]) {
        return false;
      }
    }

    return true;
  }

  S mag() {
    S sum = 0;
    for (size_t i = 0; i < D; i++) {
      sum += m_data[i] * m_data[i];
    }
    return sqrt(sum);
  }

  void norm() {
    S len = mag();

    for (size_t i = 0; i < D; i++) {
      m_data[i] /= len;
    }
  }

  S angle() {
    return atan2(m_data[1], m_data[0]); // atan2(y, x)
  }

  S angle(Vector<D> vec) {
    float cos_theta = dot(vec);
    if (cos_theta > 1.0)
      cos_theta = 1.0;
    else if (cos_theta < -1.0)
      cos_theta = -1.0;
    return acos(cos_theta);
  }

  S &operator[](size_t i) { return m_data[i]; }

  void operator+=(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] += scalar;
    }
  }

  void operator+=(Vector<D, S> vec) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] += vec.m_data[i];
    }
  }

  Vector<D, S> operator+(Vector<D, S> vec) {
    Vector<D, S> v(*this);
    v += vec;
    return v;
  }

  void operator-=(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] -= scalar;
    }
  }

  Vector<D, S> operator-(Vector<D, S> o) {
    Vector<D, S> vec;

    for (size_t i = 0; i < D; i++) {
      vec[i] = m_data[i] - o[i];
    }

    return vec;
  }

  void operator*=(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] *= scalar;
    }
  }

  Vector<D, S> operator*(S scalar) {
    Vector<D, S> vec(*this);
    vec *= scalar;
    return vec;
  }

  void print() {
    for (size_t i = 0; i < D; i++) {
      std::cout << m_data[i] << ' ';
    }
    std::cout << '\n';
  }

  S sum() {
    S sum = 0;
    for (size_t i = 0; i < D; i++) {
      sum += m_data[i];
    }
  }

  S dot(Vector<D, S> vec) {
    S sum = 0;
    for (size_t i = 0; i < D; i++) {
      sum += vec[i] * m_data[i];
    }
    return sum;
  }

private:
  S m_data[D] = {0};
};
} // namespace Math
} // namespace Engine

#endif
