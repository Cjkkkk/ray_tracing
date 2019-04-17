//
// Created by DELL on 2019/2/26.
//

#ifndef RAY_TRACING_S_RANDOM_H
#define RAY_TRACING_S_RANDOM_H

#include <cstdlib>

inline float drand48(){
    return ((float) rand() / RAND_MAX);
}
#endif //RAY_TRACING_S_RANDOM_H
