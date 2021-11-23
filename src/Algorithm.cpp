//
// Created by Fabian Vidounig on 18.11.21.
//

#include <math.h>
#include <thread>
#include <vector>

#include "Vector.h"
#include "benchmark.h"

struct RGB {
    RGB(unsigned char R=0,unsigned char G=0,unsigned char B=0): R(R),G(G),B(B) {};

    unsigned char R=0;
    unsigned char G=0;
    unsigned char B=0;
};

RGB HSVtoRGB(float H, float S,float V){
    if(H>360 || H<0 || S>100 || S<0 || V>100 || V<0){
        return RGB();
    }
    float s = S/100;
    float v = V/100;
    float C = s*v;
    float X = C*(1-abs(fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }

    return RGB((r+m)*255,(g+m)*255,(b+m)*255);
}

namespace Algorithm {
    double map(int point, int pixelWidth, double min, double max){
        return (point/(pixelWidth*1.0f)*((max-min)) + min);
    }

    void mandelbrot_calculator(int *iterations, int *iteration_map, Vector2D<int> resolution , Vector2x2<int> renderView , Vector2x2<double> view, int depth) {

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

    void mandelbrot_renderer(unsigned char *image,int *iterations, int *iteration_map, Vector2D<int> resolution , Vector2x2<int> renderView, int depth, int total) {

        for (int y = renderView.y1; y < renderView.y2; ++y) {
            if(y<0)
                continue;

            if(y>=resolution.y)
                break;

            for (int x = renderView.x1; x < renderView.x2; ++x) {

                if(x<0)
                    continue;

                if(x>=resolution.x)
                    break;

                double hue = 0;

                for (int i = 0; i < iteration_map[y*resolution.x+x] && iteration_map[y*resolution.x+x]<=depth; i++) {
                    hue += iterations[i]/(total*1.0f);
                }

                RGB rgb = HSVtoRGB(20+hue*60,100, iteration_map[y*resolution.x+x]==100? 0 : 100);
                image[y*resolution.x*3+x*3] = rgb.R;
                image[y*resolution.x*3+x*3+1] = rgb.G;
                image[y*resolution.x*3+x*3+2] = rgb.B;

            }
        }
    }

    void Mandelbrot(unsigned char *image, Vector2D<int> resolution = Vector2D<int>(1200,800), int depth=100, Vector2x2<double> view = Vector2x2<double>(-2,-1,1,1)) {
        ScopedTimer sc("Mandelbrot alg");
        DestinctTimer dt1("Calculating");
        DestinctTimer dt2("Coloring");

        dt1.start();
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
            processes.push_back(std::thread(mandelbrot_calculator, iterations,iteration_map, resolution ,Vector2x2<int>(int(width_fraction*(i-1))-1,0,int(width_fraction*i)+1,resolution.y) ,view, depth));

        for(auto &t : processes)
            t.join();

        dt1.stop();
        processes.clear();


        dt2.start();

        for(int i=0; i<=depth;i++)
            total += iterations[i];

        for(int i = 1; i<=processor_count;i++)
            processes.push_back(std::thread(mandelbrot_renderer, image,iterations,iteration_map,resolution,Vector2x2<int>(int(width_fraction*(i-1))-1,0,int(width_fraction*i)+1,resolution.y),depth,total));

        for(auto &t : processes)
            t.join();

        dt2.stop();

        delete[] iteration_map;
        delete[] iterations;
    }
}