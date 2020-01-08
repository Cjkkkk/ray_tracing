//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_RAY_H
#define RAY_TRACING_RAY_H

#include "vec3.h"

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class ray
{
public:
    CUDA_CALLABLE_MEMBER ray() {}
    CUDA_CALLABLE_MEMBER ray(const vec3& a, const vec3& b, float ti = 0.0) { A = a; B = b; _time = ti;}
    CUDA_CALLABLE_MEMBER inline vec3 origin() const       { return A; }
    CUDA_CALLABLE_MEMBER inline vec3 direction() const    { return B; }
    CUDA_CALLABLE_MEMBER inline vec3 point_at_parameter(float t) const { return A + t*B; }
    CUDA_CALLABLE_MEMBER inline float time() const { return _time;}

    vec3 A;
    vec3 B;
    float _time;
};


#endif //RAY_TRACING_RAY_H
