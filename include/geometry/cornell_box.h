//
// Created by DELL on 2019/2/28.
//

#ifndef RAY_TRACING_CORNEL_BOX_H
#define RAY_TRACING_CORNEL_BOX_H

#include "rectangle.h"
#include "hitable.h"
#include "hitablelist.h"
#include "flip.h"

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class cornell_box: public hitable  {
public:
    cornell_box() {}
    cornell_box(const vec3& p0, const vec3& p1, material *ptr);
    CUDA_CALLABLE_MEMBER virtual bool hit(const ray& r, float t0, float t1, hit_record& rec);
    CUDA_CALLABLE_MEMBER virtual bool bounding_box(float t0, float t1, aabb& box) const {
        box = aabb(pmin, pmax);
        return true; 
        }
    vec3 pmin, pmax;
    hitable *list_ptr;
};

cornell_box::cornell_box(const vec3& p0, const vec3& p1, material *ptr) {
    pmin = p0;
    pmax = p1;
    int i = 0 ;
    hitable **list = new hitable*[6];
    list[i++] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
    list[i++] = new flip(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
    list[i++] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
    list[i++] = new flip(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
    list[i++] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
    list[i++] = new flip(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    list_ptr = new hitable_list(list,i);
}

CUDA_CALLABLE_MEMBER
bool cornell_box::hit(const ray& r, float t0, float t1, hit_record& rec){
    return list_ptr->hit(r, t0, t1, rec);
}

#endif //RAY_TRACING_CORNEL_BOX_H
