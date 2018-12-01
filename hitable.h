//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_HITABLE_H
#define RAY_TRACING_HITABLE_H

#include "ray.h"

class material;

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

class hitable  {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};


#endif //RAY_TRACING_HITABLE_H
