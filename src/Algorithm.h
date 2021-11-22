//
// Created by Fabian Vidounig on 18.11.21.
//

#ifndef FRACTALS_ALGORITHM_H
#define FRACTALS_ALGORITHM_H

#include "Vector.h"

namespace Algorithm {
    void Mandelbrot(unsigned char *image, Vector2D<int> resolution = Vector2D<int>(1200,800), int depth=100, Vector2x2<double> view = Vector2x2<double>(-2,-1,1,1));
}

#endif  FRACTALS_ALGORITHM_H