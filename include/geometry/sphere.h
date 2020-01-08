//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_SPHERE_H
#define RAY_TRACING_SPHERE_H

#include "hitable.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class sphere: public hitable  {
public:
    sphere() {}
    sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m)  {};
    CUDA_CALLABLE_MEMBER virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec);
    CUDA_CALLABLE_MEMBER virtual bool bounding_box(float t0, float t1, aabb& box) const;
    vec3 center;
    float radius;
    material *mat_ptr;
};

CUDA_CALLABLE_MEMBER
void get_sphere_uv(vec3 &p, float &u, float &v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1 - (phi + M_PI) / ( 2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

CUDA_CALLABLE_MEMBER
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.normal, rec.u, rec.v);
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv(rec.normal, rec.u, rec.v);
            return true;
        }
    }
    return false;
}

CUDA_CALLABLE_MEMBER
bool sphere :: bounding_box(float t0, float t1, aabb& box) const {
    box = aabb( center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}
#endif //RAY_TRACING_SPHERE_H

