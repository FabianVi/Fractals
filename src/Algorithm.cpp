//
// Created by Fabian Vidounig on 18.11.21.
//

#include <math.h>
#include <thread>
#include <vector>

#include "Vector.h"
#include "benchmark.h"

namespace Algorithm {
    double map(int point, int pixelWidth, double min, double max){
        return (point/(pixelWidth*1.0f)*((max-min)) + min);
    }

    void mandelbrot_renderer(int *iterations, int *iteration_map, Vector2D<int> resolution , Vector2x2<int> renderView , Vector2x2<double> view, int depth) {

        for (int yp = renderView.y1; yp < renderView.y2; ++yp) {
            if(yp<0)
                continue;

            if(yp>=resolution.y)
                break;

             for (int xp = renderView.x1; xp < renderView.x2; ++xp) {

                 if(xp<0)
                     continue;

                 if(xp>=resolution.x)
                     break;

                double x0 = map(xp, resolution.x, view.x1, view.x2);
                double y0 = map(yp, resolution.y, view.y1, view.y2);

                double x1 = 0;
                double y1 = 0;

                double x2=0;
                double y2=0;

                int n = 0;

                do {
                    y1 = 2 * x1 * y1 + y0;
                    x1 = x2 - y2 + x0;
                    x2 = x1 * x1;
                    y2 = y1 * y1;

                    n++;
                } while(x2+y2 <= 4 && n<depth);

                iteration_map[xp+yp*resolution.x] = n;
                iterations[n]+=1;
            }
        }
    }

    void Mandelbrot(unsigned char *image, Vector2D<int> resolution = Vector2D<int>(1200,800), int depth=100, Vector2x2<double> view = Vector2x2<double>(-2,-1,1,1)) {
        ScopedTimer sc("Mandelbrot alg");

        int *iterations = new int[depth+1];
        for(int i=0; i<=depth;i++)
            iterations[i] = 0;

        int *iteration_map = new int[resolution.x*resolution.y];

        uint64_t total = 0;

        int processor_count = std::thread::hardware_concurrency();
        // int processor_count = 1;

        if(processor_count==0)
            processor_count = 1;

        double width_fraction = resolution.x/(processor_count*1.0f);

        std::vector<std::thread> processes;

        for(int i = 1; i<=processor_count;i++)
            processes.push_back(std::thread(mandelbrot_renderer, iterations,iteration_map, resolution ,Vector2x2<int>(int(width_fraction*(i-1))-1,0,int(width_fraction*i)+1,resolution.y) ,view, depth));

        for(auto &t : processes)
            t.join();

        for(int i=0; i<=depth;i++)
            total += iterations[i];

        double *hue = new double[resolution.x*resolution.y];

        for(int i=0; i<(resolution.x*resolution.y);i++)
            hue[i] = 0;


        for(int y=0; y<resolution.y;y++)
            for(int x=0; x<resolution.x;x++)
            {

                if(iteration_map[y*resolution.x+x]>depth)
                    std::cout << iteration_map[y*resolution.x+x] << std::endl;

                for (int i = 0; i < iteration_map[y*resolution.x+x] && iteration_map[y*resolution.x+x]<=depth; i++) {
                    hue[resolution.x*y+x] += iterations[i]/(total*1.0f);
                }
            }



        for(int y=0; y<resolution.y;y++)
            for(int x=0; x<resolution.x;x++)
            {
                image[y*resolution.x*3+x*3] = 255-(unsigned char)(hue[y*resolution.x+x]*255);
                image[y*resolution.x*3+x*3+1] = 255-(unsigned char)(hue[y*resolution.x+x]*255);
                image[y*resolution.x*3+x*3+2] = 255-(unsigned char)(hue[y*resolution.x+x]*255);
            }

        delete iteration_map;
        delete iterations;
        delete hue;
    }
}