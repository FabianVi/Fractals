//
// Created by Fabian Vidounig on 18.11.21.
//

#include <math.h>

namespace Algorithm {
    double map(int point, int pixelWidth, double min, double max){
        return (point*((max-min)/pixelWidth) + min);
    }

    void Mandelbrot(unsigned char *image, int width=1200, int height=800, double minX=-2, double minY=-1, double maxX=1, double maxY=1) {
        for (int xp = 0; xp < width; ++xp) {
            for (int yp = 0; yp < height; ++yp) {
                double cRe = map(xp, width, minX, maxX);
                double cIm = map(yp, height,minY,maxY);

                double zRe = cRe;
                double zIm = cIm;
                double zRePrev = cRe;
                double zImPrev = cIm;
                int n;
                for (n = 0; n < 255; ++n) {
                    zRePrev = zRe*zRe - zIm*zIm; //zRePrev*zRePrev-zImPrev*zImPrev
                    zImPrev=2*zRe*zIm;

                    zRe=zRePrev+cRe;
                    zIm=zImPrev+cIm;

                    if(sqrt(abs(zRe+zIm)) > 16){
                        break;
                    }
                }

                image[width*3*yp+xp*3] = n;
                image[width*3*yp+xp*3+1] = n;
                image[width*3*yp+xp*3+2] = n;
            }
        }
    }
}

