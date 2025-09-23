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

  size_t size(){
	return D * sizeof(S);
  }

  S mag(){
	S sum = 0;
	for (size_t i = 0; i < D; i++) {
		sum += m_data[i] * m_data[i];
	}
	return sqrt(sum);
  }

  void norm(){
	  S len = mag();

	  for (size_t i = 0; i < D; i++) {
		  m_data[i] /= len;
	  }
  }

  S angle(Vector<D> vec){
	  float cos_theta = dot(vec);
	  if (cos_theta > 1.0) cos_theta = 1.0;
	  else if (cos_theta < -1.0) cos_theta = -1.0;
	  return acos(cos_theta);
  }

  S &operator[](size_t i) { return m_data[i]; }

  Vector operator+=(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] += scalar;
    }
  }

  Vector operator-=(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] -= scalar;
    }
  }

  Vector operator-(Vector<D> o) {
	Vector vec;

    for (size_t i = 0; i < D; i++) {
		vec[i] = m_data[i] -  o[i];
    }

	return vec;
  }

  Vector operator*=(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] -= scalar;
    }
  }

  void print(){
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
