//
// Created by Fabian Vidounig on 19.11.21.
//

#include "benchmark.h"

uint64_t getCurrentMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    ).count();
}
