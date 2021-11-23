//
// Created by Fabian Vidounig on 19.11.21.
//

#ifndef FRACTALS_VECTOR_H
#define FRACTALS_VECTOR_H


template <typename T>
struct Vector2D {
    Vector2D(T x=0, T y=0) : x(x),y(y){};

    T x;
    T y;
};

template <typename T>
struct Vector2x2 {
    Vector2x2(T x1=0, T y1=0,T x2=0, T y2=0) : x1(x1),y1(y1),x2(x2),y2(y2){};

    T x1;
    T y1;
    T x2;
    T y2;
};

#endif