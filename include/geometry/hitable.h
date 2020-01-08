//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_HITABLE_H
#define RAY_TRACING_HITABLE_H

#include <math.h>
#include "ray.h"

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class material;

struct hit_record
{
    float t;
    float u;
    float v;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

inline float fminf(float a, float b) { return a < b ? a : b;}
inline float fmaxf(float a, float b) { return a > b ? a : b;}

class aabb {
public:
    CUDA_CALLABLE_MEMBER aabb(){}
    CUDA_CALLABLE_MEMBER aabb(const vec3& a, const vec3& b) : _min(a), _max(b) {
        _center = vec3((a.x() + b.x()) / 2,
                (a.y() + b.y()) / 2,
                (a.z() + b.z()) / 2);
    }
    CUDA_CALLABLE_MEMBER int getSubSlotIndex(const vec3& pos) const {
        bool back = pos.z() > _center.z();
        bool left = pos.x() > _center.x();
        bool up = pos.y() < _center.y();
        return up * 4 + back * 2 + left * 1;
    }
    CUDA_CALLABLE_MEMBER aabb* getSubSlot(const int index){
        vec3 pMin, pMax;
        pMin[0] = (index & 1) ? center()[0] : min()[0];
        pMax[0] = (index & 1) ? max()[0] : center()[0];
        pMin[1] = (index & 4) ? min()[1] : center()[1];
        pMax[1] = (index & 4) ? center()[1] : max()[1];
        pMin[2] = (index & 2) ? center()[2] : min()[2];
        pMax[2] = (index & 2) ? max()[2] : center()[2];
        return new aabb(pMin, pMax);
    }
    CUDA_CALLABLE_MEMBER vec3 max() { return _max; }
    CUDA_CALLABLE_MEMBER vec3 min() { return _min; }
    CUDA_CALLABLE_MEMBER vec3 center() { return _center;}
    CUDA_CALLABLE_MEMBER bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
        for(int a = 0 ; a < 3 ; a++){
            float t0 = fminf(
                    (_min[a] - r.origin()[a]) / r.direction()[a],
                    (_max[a] - r.origin()[a]) / r.direction()[a]
            );
            float t1 = fmaxf(
                    (_min[a] - r.origin()[a]) / r.direction()[a],
                    (_max[a] - r.origin()[a]) / r.direction()[a]
            );

            tmin = fmaxf(t0, tmin);
            tmax = fminf(t1, tmax);
            if(tmax <= tmin) return false;
        }
        rec.t = tmin;
        return true;
    }
    vec3 _min;
    vec3 _max;
    vec3 _center;
};

CUDA_CALLABLE_MEMBER
aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small(
            fminf(box0.min().x(), box1.min().x()),
            fminf(box0.min().y(), box1.min().y()),
            fminf(box0.min().z(), box1.min().z()));
    vec3 big(
            fmaxf(box0.max().x(), box1.max().x()),
            fmaxf(box0.max().y(), box1.max().y()),
            fmaxf(box0.max().z(), box1.max().z()));
    return aabb(small, big);
}

class hitable  {
public:
    CUDA_CALLABLE_MEMBER virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) = 0;
    CUDA_CALLABLE_MEMBER virtual bool bounding_box(float t0, float t1, aabb& box) const = 0; // 获得物体的bounding cornell_box
};


#endif //RAY_TRACING_HITABLE_H
