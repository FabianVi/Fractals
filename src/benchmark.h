//
// Created by Fabian Vidounig on 19.11.21.
//

#ifndef FRACTALS_BENCHMARK_H
#define FRACTALS_BENCHMARK_H

#include <chrono>
#include <iostream>
#include <string>

uint64_t getCurrentMillis();

class ScopedTimer {
private:
    uint64_t ms;
    std::string name;

public:
    ScopedTimer(std::string name = "timer") {
        this->ms = getCurrentMillis();
        this->name = name;
    }
    ~ScopedTimer() {

        #ifdef benchmark
        std::cout << "Timer : " <<  name << " : " << (getCurrentMillis() - ms) << "ms" << std::endl;
        #endif
    }
};

class DestinctTimer {
    uint64_t ms;
    std::string name;

public:
    DestinctTimer(std::string name = "timer") {
        this->name = name;
    }

    void start() {
        this->ms = getCurrentMillis();
    }
    void stop() {
        #ifdef benchmark
        std::cout << "Timer : " <<  name << " : " << (getCurrentMillis() - ms) << "ms" << std::endl;
        #endif
    }
};
#endif FRACTALS_BENCHMARK_H
