//
// Created by Fabian Vidounig on 18.11.21.
//

#include <math.h>;
#include "Vector.h";

namespace Algorithm {
    double map(int point, int pixelWidth, double min, double max){
        return (point*((max-min)/pixelWidth) + min);
    }

    void mandelbrot_renderer(unsigned char *image, Vector2D<int> resolution , Vector2x2<int> renderView , Vector2x2<double> view) {
        for (int xp = renderView.x1; xp < renderView.x2; ++xp) {
            for (int yp = renderView.y1; yp < renderView.y2; ++yp) {
                double cRe = map(xp, resolution.x, view.x1, view.x2);
                double cIm = map(yp, resolution.y, view.y1, view.y2);

                double zRe = cRe;
                double zIm = cIm;
                double zRePrev = cRe;
                double zImPrev = cIm;
                int n;
                for (n = 0; n < 100; ++n) {
                    zRePrev = zRe*zRe - zIm*zIm; //zRePrev*zRePrev-zImPrev*zImPrev
                    zImPrev=2*zRe*zIm;

                    zRe=zRePrev+cRe;
                    zIm=zImPrev+cIm;

                    if(sqrt(abs(zRe+zIm)) > 16){
                        break;
                    }
                }

                image[resolution.x*3*yp+xp*3] = n*2.5f;
                image[resolution.x*3*yp+xp*3+1] = n*2.5f;
                image[resolution.x*3*yp+xp*3+2] = n*2.5f;
            }
        }
    }

    void Mandelbrot(unsigned char *image, Vector2D<int> resolution = Vector2D<int>(1200,800), Vector2x2<double> view = Vector2x2<double>(-2,-1,1,1)) {
        mandelbrot_renderer(image , resolution ,Vector2x2<int>(0,0,resolution.x,resolution.y) ,view);
    }
}