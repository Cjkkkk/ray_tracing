//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_HITABLE_H
#define RAY_TRACING_HITABLE_H

#include "../ray.h"

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

inline float ffmin(float a, float b) { return a < b ? a : b;}
inline float ffmax(float a, float b) { return a > b ? a : b;}

class aabb {
public:
    aabb(){}
    aabb(const vec3& a, const vec3& b) : _min(a), _max(b) {}
    vec3 max() { return _max; }
    vec3 min() { return _min; }
    bool hit(const ray& r, float tmin, float tmax) const {
        for(int a = 0 ; a < 3 ; a++){
            float t0 = ffmin(
                    (_min[a] - r.origin()[a]) / r.direction()[a],
                    (_max[a] - r.origin()[a]) / r.direction()[a]
            );
            float t1 = ffmax(
                    (_min[a] - r.origin()[a]) / r.direction()[a],
                    (_max[a] - r.origin()[a]) / r.direction()[a]
            );

            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            if(tmax <= tmin) return false;
        }
        return true;
    }
    vec3 _min;
    vec3 _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small(
            ffmin(box0.min().x(), box1.min().x()),
            ffmin(box0.min().y(), box1.min().y()),
            ffmin(box0.min().z(), box1.min().z()));
    vec3 big(
            ffmax(box0.max().x(), box1.max().x()),
            ffmax(box0.max().y(), box1.max().y()),
            ffmax(box0.max().z(), box1.max().z()));
    return aabb(small, big);
}
class hitable  {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};


#endif //RAY_TRACING_HITABLE_H
