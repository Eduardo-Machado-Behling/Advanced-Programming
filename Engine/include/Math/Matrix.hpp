#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <Math/Vector.hpp>

namespace Engine {
namespace Math {

template <size_t COLS, size_t ROWS, typename S> class Matrix {
public:
	Matrix(){
		const size_t size = COLS * ROWS;
		for (size_t i = 0; i < size; i++) {
			if (i % COLS == i)
				data[i] = 1;
			else 
				data[i] = 0;
		}
	}

	Matrix mult(Matrix& matrix){
		for (size_t i = 0; i < ROWS; i++){
			for (size_t j = 0; j < COLS; j++){
				Vector<COLS, S> col;
			}
		}
	}

private:
  S data[ROWS * COLS];
};
} // namespace Math

} // namespace Engine

#endif
