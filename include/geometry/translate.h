//
// Created by DELL on 2019/2/28.
//

#ifndef RAY_TRACING_TRANSLATE_H
#define RAY_TRACING_TRANSLATE_H

#include "hitable.h"

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class translate : public hitable {
public:
    translate(hitable *p, const vec3& displacement) : ptr(p), offset(displacement) {}
    CUDA_CALLABLE_MEMBER virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec);
    CUDA_CALLABLE_MEMBER virtual bool bounding_box(float t0, float t1, aabb& box) const;
    hitable *ptr;
    vec3 offset;
};

CUDA_CALLABLE_MEMBER
bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (ptr->hit(moved_r, t_min, t_max, rec)) {
        rec.p += offset;
        return true;
    }
    else
        return false;
}

CUDA_CALLABLE_MEMBER
bool translate::bounding_box(float t0, float t1, aabb& box) const {
    if (ptr->bounding_box(t0, t1, box)) {
        box = aabb(box.min() + offset, box.max()+offset);
        return true;
    }
    else
        return false;
}
#endif //RAY_TRACING_TRANSLATE_H
