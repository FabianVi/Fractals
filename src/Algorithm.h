//
// Created by Fabian Vidounig on 18.11.21.
//

#ifndef FRACTALS_ALGORITHM_H
#define FRACTALS_ALGORITHM_H

#include "Vector.h"

namespace Algorithm {
    void Fractal(unsigned char *image, Vector2D<int> resolution = Vector2D<int>(1200, 800), int depth = 100,
                 Vector2x2<long double> view = Vector2x2<long double>(-2, -1, 1, 1));

    long double map(int point, int pixelWidth, long double min, long double max);

    Vector2x2<long double> zoom(Vector2x2<long double> canvas,Vector2D<long double> origin, long double zoomFactor, long double translation_factor=0.L);
    Vector2x2<long double> zoom(Vector2x2<long double> canvas, long double zoomFactor);
}
#endif