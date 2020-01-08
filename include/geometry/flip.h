//
// Created by DELL on 2019/2/28.
//

#ifndef RAY_TRACING_FLIP_H
#define RAY_TRACING_FLIP_H

#include "hitable.h"

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class flip: public hitable{
public:
    flip(hitable* _h): h(_h){}
    CUDA_CALLABLE_MEMBER virtual bool hit(const ray& r, float t0, float t1, hit_record& rec);
    CUDA_CALLABLE_MEMBER virtual bool bounding_box(float t0, float t1, aabb &box) const;
    hitable* h;
};

CUDA_CALLABLE_MEMBER
bool flip::hit(const ray &r, float t0, float t1, hit_record &rec){
    if(h->hit(r, t0, t1, rec)){
        rec.normal = -rec.normal;
        return true;
    };
    return false;
}

CUDA_CALLABLE_MEMBER
bool flip::bounding_box(float t0, float t1, aabb &box) const {
    return h->bounding_box(t0, t1, box);
}
#endif //RAY_TRACING_FLIP_H
