//
// Created by DELL on 2019/3/11.
//

#ifndef RAY_TRACING_TRIANGLE_H
#define RAY_TRACING_TRIANGLE_H


#include "hitable.h"

class triangle: public hitable  {
public:
    triangle() {}
    triangle(vec3 v0_, vec3 v1_, vec3 v2_, material *m) : v0(v0_), v1(v1_), v2(v2_), mat_ptr(m){
        N = cross(v1 - v0, v2 - v0);
        N.make_unit_vector();
        D = dot(N, v0);
    };
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec);
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    vec3 v0, v1, v2, N;
    float D;
    material *mat_ptr;
};


bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) {
    // 判断是否ray与平面平行
    hitable::increaseIntersectionTimes();
    float result = dot(N, r.direction());
    if(std::abs(result) < 1e-6) return false;
    // 求与平面相交时间
    float t = (dot(N, r.origin()) + D) / (- dot(N, r.direction()));
    if (t < t_max && t > t_min){
        // inside outside 算法
        vec3 p = r.point_at_parameter(t);
        if( (dot(cross(v1 - v0, p - v0), N) > 0)
        && (dot(cross(v2 - v1, p - v1), N) > 0)
        && (dot(cross(v0 - v2, p - v2), N) > 0) ) {
            rec.p = p;
            rec.normal = N;
            rec.mat_ptr = mat_ptr;
            rec.t = t;
//            vec3 middle = (v1 + v2) / 2;
//            auto middle_length = (middle - v0).length();
//            rec.u = (middle - p).length() / middle_length;
            return true;
        }
    }
    return false;
}

bool triangle:: bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(vec3(
            std::min(std::min(v0.x(), v1.x()), v2.x()),
            std::min(std::min(v0.y(), v1.y()), v2.y()),
            std::min(std::min(v0.z(), v1.z()), v2.z()) - 0.0001
            ), vec3(
            std::max(std::max(v0.x(), v1.x()), v2.x()),
            std::max(std::max(v0.y(), v1.y()), v2.y()),
            std::max(std::max(v0.z(), v1.z()), v2.z()) + 0.0001
            ));
    return true;
}


#endif //RAY_TRACING_TRIANGLE_H
