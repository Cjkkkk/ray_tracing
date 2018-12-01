//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_SPHERE_H
#define RAY_TRACING_SPHERE_H

#include "hitable.h"
class sphere:public hitable {
public:
    sphere(){}
    sphere(vec3 cen, float r, material* mat): center(cen), radius(r), mat_ptr(mat){}
    vec3 center;
    float radius;
    material *mat_ptr;
    virtual bool hit(const ray&r, float tmin, float tmax, hit_record& rec) const{
        vec3 oc = r.origin() - center; // A - C  P = A + t*B
        float a = dot(r.direction(), r.direction());
        float b = 2.0 * dot(oc, r.direction());
        float c = dot(oc, oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        if(discriminant > 0){
            float temp = (-b - sqrt(b * b - 4 * a * c))/a;
            if(temp < tmax && temp > tmin){
                rec.t = temp;
                rec.p = r.point_at_parameter(temp);
                rec.normal = (rec.p - center) / radius;
                rec.mat_ptr = mat_ptr;
                return true;
            }
            temp = (-b + sqrt(b * b - 4 * a * c))/a;
            if(temp < tmax && temp > tmin){
                rec.t = temp;
                rec.p = r.point_at_parameter(temp);
                rec.normal = (rec.p - center) / radius;
                rec.mat_ptr = mat_ptr;
                return true;
            }
        }
        return false;
    }
};


//bool sphere::hit(const ray&r, float tmin, float tmax, hit_record& rec) const{
//    vec3 oc = r.origin() - center; // A - C  P = A + t*B
//    float a = dot(r.direction(), r.direction());
//    float b = 2.0 * dot(oc, r.direction());
//    float c = dot(oc, oc) - radius * radius;
//    float discriminant = b * b - 4 * a * c;
//    if(discriminant > 0){
//        float temp = (-b - sqrt(b*b-a*c))/a;
//        if(temp < tmax && temp > tmin){
//            rec.t = temp;
//            rec.p = r.point_at_parameter(temp);
//            rec.normal = (rec.p - center) / radius;
//            return true;
//        }
//        temp = (-b + sqrt(b*b-a*c))/a;
//        if(temp < tmax && temp > tmin){
//            rec.t = temp;
//            rec.p = r.point_at_parameter(temp);
//            rec.normal = (rec.p - center) / radius;
//            return true;
//        }
//    }
//    return false;
//};
#endif //RAY_TRACING_SPHERE_H
