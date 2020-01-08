//
// Created by DELL on 2019/2/26.
//

#ifndef RAY_TRACING_S_RANDOM_H
#define RAY_TRACING_S_RANDOM_H

#include <cstdlib>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
inline float drand48(){
    return ((float) rand() / RAND_MAX);
}
#endif

#endif //RAY_TRACING_S_RANDOM_H
