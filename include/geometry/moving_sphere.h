//
// Created by DELL on 2019/2/26.
//

#ifndef RAY_TRACING_MOVING_SPHERE_H
#define RAY_TRACING_MOVING_SPHERE_H

#include "hitable.h"

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class moving_sphere: public hitable {
public:
    moving_sphere(){}
    moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material *m):
            center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {};
    CUDA_CALLABLE_MEMBER vec3 center(float time) const;
    CUDA_CALLABLE_MEMBER virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) ;
    CUDA_CALLABLE_MEMBER virtual bool bounding_box(float t0, float t1, aabb& box) const;

    float time0, time1, radius;
    vec3 center0, center1;
    material* mat_ptr;
};

CUDA_CALLABLE_MEMBER
vec3 moving_sphere::center(float time) const {
    return center0 + (center1 - center0) * (time - time0) / (time1 - time0);
}

CUDA_CALLABLE_MEMBER 
bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) {
//    number_of_ray_object_test += 1;
    vec3 oc = r.origin() - center(r.time());
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

CUDA_CALLABLE_MEMBER 
bool moving_sphere :: bounding_box(float t0, float t1, aabb& box) const {
    aabb box0 = aabb( center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
    aabb box1 = aabb( center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
    box = surrounding_box(box0, box1);
    return true;
}
#endif //RAY_TRACING_MOVING_SPHERE_H
