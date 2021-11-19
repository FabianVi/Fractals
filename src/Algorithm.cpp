//
// Created by Fabian Vidounig on 18.11.21.
//

#include <math.h>;
#include <thread>;
#include <vector>
#include <iostream>;

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

                for (n = 0; n < 255; ++n) {
                    zRePrev = zRe*zRe - zIm*zIm; //zRePrev*zRePrev-zImPrev*zImPrev
                    zImPrev=2*zRe*zIm;

                    zRe=zRePrev+cRe;
                    zIm=zImPrev+cIm;

                    if(sqrt(abs(zRe+zIm)) > 16){
                        break;
                    }
                }

                // coloring
                if(n<10) {
                    image[resolution.x*3*yp+xp*3] = 0;
                    image[resolution.x*3*yp+xp*3+1] = 25*n;
                    image[resolution.x*3*yp+xp*3+2] = 25*n;
                }else
                if(n<20) {
                    image[resolution.x*3*yp+xp*3] = 0;
                    image[resolution.x*3*yp+xp*3+1] = 25*(n-10);
                    image[resolution.x*3*yp+xp*3+2] = 25*(n-10);
                } else if(n<50) {
                    image[resolution.x*3*yp+xp*3] = 0;
                    image[resolution.x*3*yp+xp*3+1] = int(254*(n/50.0f));
                    image[resolution.x*3*yp+xp*3+2] = int(254*(n/50.0f));
                } else {
                    image[resolution.x*3*yp+xp*3] = 255-n;
                    image[resolution.x*3*yp+xp*3+1] = 255-n;
                    image[resolution.x*3*yp+xp*3+2] = 255-n;
                }
            }
        }
    }

    void Mandelbrot(unsigned char *image, Vector2D<int> resolution = Vector2D<int>(1200,800), Vector2x2<double> view = Vector2x2<double>(-2,-1,1,1)) {
        int processor_count = std::thread::hardware_concurrency();
        //int processor_count = 1;

        if(processor_count==0)
            processor_count = 1;

        double width_fraction = resolution.x/(processor_count*1.0f);

        std::vector<std::thread> processes;

        for(int i = 1; i<=processor_count;i++)
            processes.push_back(std::thread(mandelbrot_renderer,image , resolution ,Vector2x2<int>(int(width_fraction*(i-1)),0,int(width_fraction*i),resolution.y) ,view));

        for(auto &t : processes)
            t.join();

        std::cout << processor_count << std::endl;
    }
}