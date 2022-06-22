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
    enum Algo {
        Mandelbrot,
        BurningShip
    };

    /**
     * This Function can zoom and translate to a given origin (Vector2D) and adapt a 2x2 vector, which represents the canvas size, to this new Framing.
     * @param canvas Vector2x2 will be translated and zoomed
     * @param origin The Zoom and Rotation center point
     * @param zoomFactor The level of zoom e.g. zoom in 0.5 or zoom out 2
     * @param translation_factor The Coefficient of Translation e.g. 1 if you want the Origin be the center of frame
     * @return Vector2x2 the new Framing
     */
    Vector2x2<long double> zoom(Vector2x2<long double> canvas,Vector2D<long double> origin, long double zoomFactor, long double translation_factor=0.L){
        long double dx1 = origin.x-canvas.x1;
        long double dx2 = canvas.x2 - origin.x;

        dx1*=zoomFactor;
        dx2*=zoomFactor;

        long double dy1 = origin.y-canvas.y1;
        long double dy2 = canvas.y2-origin.y;

        dy1*=zoomFactor;
        dy2*=zoomFactor;

        Vector2x2 newCanvas(origin.x-dx1,origin.y-dy1,origin.x+dx2,origin.y+dy2);

        long double dx =  (origin.x-(newCanvas.x1+(newCanvas.x2-newCanvas.x1)/2.0F)) * translation_factor;
        long double dy =  (origin.y - (newCanvas.y1+(newCanvas.y2-newCanvas.y1)/2.0F)) * translation_factor;

        newCanvas.x1 += dx;
        newCanvas.x2 += dx;
        newCanvas.y1 += dy;
        newCanvas.y2 += dy;

        return newCanvas;
    }

    /**
     * Zooms to the center of Frame by the given zoomFactor
     * @param canvas Vector2x2 will be translated and zoomed
     * @param zoomFactor The level of zoom e.g. zoom in 0.5 or zoom out 2
     * @return Vector2x2 the new Framing
     */
    Vector2x2<long double> zoom(Vector2x2<long double> canvas, long double zoomFactor){
        long double dx = (canvas.x2-canvas.x1)/2.0L;
        long double dy = (canvas.y2-canvas.y1)/2.0L;

        return zoom(canvas, Vector2D(canvas.x1+dx,canvas.y1+dy),zoomFactor,0.0L);
    }

    long double map(int point, int pixelWidth, long double min, long double max){
        return (point/(pixelWidth*1.0f)*(max-min)) + min;
    }

    int mandelbrot(long double x ,long double y , int depth) {
        long double x1 = 0;
        long double y1 = 0;

        long double x2=0;
        long double y2=0;

        int n = 0;

        do {
            y1 = 2 * x1 * y1 + y;
            x1 = x2 - y2 + x;
            x2 = x1 * x1;
            y2 = y1 * y1;

            n++;
        } while(x2+y2 <= 4 && n<depth);

        return n;
    }

    // Z_(n+1) = (|Re(Z_n)| + |Im(Z_n)|j)^2 + c
    int burningShip (long double x ,long double y , int depth) {

        long double jx = x;
        long double jy = y;

        int n = 0;

        do {
            long double xTemp = jx*jx - jy*jy + x;
            jy = std::abs(2*jx*jy) + y; // abs gibt den absoluten Wert aus
            jx = xTemp;

            n++;
        } while(jx+jy <= 4 && n<depth);

        return n;
    }

    void depth_map (Algo o,int *iterations, int *iteration_map, Vector2D<int> resolution , Vector2x2<int> renderView , Vector2x2<long double> view, int depth) {

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

                long double x = map(xp, resolution.x, view.x1, view.x2);
                long double y = map(yp, resolution.y, view.y1, view.y2);

                int n;

                switch (o) {
                    case Algo::Mandelbrot:
                        n = mandelbrot(x,y,depth);
                        break;
                    case Algo::BurningShip:
                        n = burningShip(x,y,depth);
                        break;
                    default:
                        break;
                }

                iteration_map[xp+yp*resolution.x] = n;
                iterations[n]+=1;
            }
            // std::cout << yp << " : finished " << std::endl;
        }
    }

    void color_map(unsigned char *image,int *iterations, int *iteration_map, Vector2D<int> resolution , Vector2x2<int> renderView, int depth, int total, Vector2D<double> colouring) {

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

                RGB rgb = HSVtoRGB(colouring.x+hue*colouring.y,100, iteration_map[y*resolution.x+x]==depth? 0 : 100);
                image[y*resolution.x*3+x*3] = rgb.R;
                image[y*resolution.x*3+x*3+1] = rgb.G;
                image[y*resolution.x*3+x*3+2] = rgb.B;

            }
        }
    }

    void Fractal(Algo o, unsigned char *image, Vector2D<int> resolution = Vector2D<int>(1200,800), int depth=100, Vector2x2<long double> view = Vector2x2<long double>(-2,-1,1,1),Vector2D<double> colouring = Vector2D<double>(20,60)) {
        ScopedTimer algo_timer("All Calc/Color");
        DestinctTimer calculation_timer("Calculating");
        DestinctTimer coloring_timer("Coloring");

        calculation_timer.start();
        int *iterations = new int[depth+1];
        for(int i=0; i<=depth;i++)
            iterations[i] = 0;

        int *iteration_map = new int[resolution.x*resolution.y];

        uint64_t total = 0;

        int processor_count = std::thread::hardware_concurrency();
        // std::cout << "count : " << processor_count << std::endl;
        // int processor_count = 1;

        if(processor_count==0)
            processor_count = 1;

        double width_fraction = resolution.x/(processor_count*1.0f);

        std::vector<std::thread> processes;

        for(int i = 1; i<=processor_count;i++)
            processes.push_back(std::thread(depth_map,o, iterations,iteration_map, resolution ,Vector2x2<int>(int(width_fraction*(i-1))-1,0,int(width_fraction*i)+1,resolution.y) ,view, depth));

        for(auto &t : processes)
            t.join();

        calculation_timer.stop();

        processes.clear();

        coloring_timer.start();

        for(int i=0; i<=depth;i++)
            total += iterations[i];

        for(int i = 1; i<=processor_count;i++)
            processes.push_back(std::thread(color_map, image,iterations,iteration_map,resolution,Vector2x2<int>(int(width_fraction*(i-1))-1,0,int(width_fraction*i)+1,resolution.y),depth,total,colouring));

        for(auto &t : processes)
            t.join();

        coloring_timer.stop();

        delete[] iteration_map;
        delete[] iterations;
    }
}