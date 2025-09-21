#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cstddef>
#include <initializer_list>

namespace Engine {
namespace Math {

template <size_t D, typename S = float> class Vector {
public:
  Vector(S* data): m_data(data), m_owns(false){
  }

  ~Vector(){
	if(m_owns)
		delete m_data;
  }

  Vector(std::initializer_list<S> data) : m_owns(true) { 
	m_data = new float[D];

    for (size_t i = 0; i < D; i++) {
      m_data[i] = data[i];
    }
  }

  
  Vector(S scalar) : m_owns(true) {
	m_data = new float[D];

    for (size_t i = 0; i < D; i++) {
      m_data[i] = scalar;
    }
  }

  Vector() : m_owns(true) {
	m_data = new float[D];

    for (size_t i = 0; i < D; i++) {
      m_data[i] = 0;
    }
  }

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

  Vector operator*=(S scalar) {
    for (size_t i = 0; i < D; i++) {
      m_data[i] -= scalar;
    }
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
  S* m_data;
  bool m_owns;
};
} // namespace Math
} // namespace Engine

#endif
