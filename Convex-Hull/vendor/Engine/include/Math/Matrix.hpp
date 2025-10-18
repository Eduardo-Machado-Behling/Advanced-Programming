#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <Math/Vector.hpp>
#include <iostream>

namespace Engine {
namespace Math {

template <size_t COLS, size_t ROWS, typename S = float> class Matrix {
public:
  Matrix() {
    const size_t size = COLS * ROWS;

    size_t id = 0;
    for (size_t i = 0; i < ROWS; i++) {
      m_data[id] = 1;
      id += COLS + 1;
    }
  }

  void print() {
    for (size_t i = 0; i < ROWS; i++) {
      for (size_t j = 0; j < COLS; j++) {
        std::cout << at(i, j) << ' ';
      }
      std::cout << '\n';
    }
  }
  void identity() {
    for (size_t i = 0; i < ROWS * COLS; i++) {
      m_data[i] = (i % (COLS + 1) == 0) ? 1 : 0;
    }
  }

  size_t size() { return COLS * ROWS * sizeof(S); }

  Matrix mult(Matrix &matrix) {}

  void model(Vector<2, S> pos, Vector<2, S> scale, float rot = 0) {
    //! Assumes original Indentity

    float c = cos(rot);
    float s = sin(rot);

    // Column Major 4x4 matrix
    m_data[0] = c * scale[0];
    m_data[1] = s * scale[0];
    m_data[4] = -s * scale[1];
    m_data[5] = c * scale[1];
    m_data[12] = pos[0];
    m_data[13] = pos[1];
  }

  S &operator[](size_t i) { return m_data[i]; }

  S &at(size_t i, size_t j) { return m_data[i + j * COLS]; }

  static Matrix<4, 4, S> Ortho(S x0, S x1, S y0, S y1, S z0, S z1) {
    Matrix<4, 4, S> mat;

    mat.m_data[0] = 2 / (x1 - x0);
    mat.m_data[5] = 2 / (y1 - y0);
    mat.m_data[10] = -2 / (z1 - z0);

    mat.m_data[12] = -(x1 + x0) / (x1 - x0);
    mat.m_data[13] = -(y1 + y0) / (y1 - y0);
    mat.m_data[14] = -(z1 + z0) / (z1 - z0);
    mat.m_data[15] = 1;

    return mat;
  }

private:
  S m_data[ROWS * COLS] = {0};
};
} // namespace Math
} // namespace Engine

#endif
