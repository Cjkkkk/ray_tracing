//
// Created by DELL on 2019/2/28.
//

#ifndef RAY_TRACING_RECTANGLE_H
#define RAY_TRACING_RECTANGLE_H

#include "hitable.h"

class xy_rect : public hitable {
public:
    xy_rect() {}
    xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, material *mat):x0(_x0),x1(_x1),y0(_y0), y1(_y1), k(_k), mp(mat){};
    float x0, x1, y0, y1, k;
    material* mp;
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const {
        box = aabb(vec3(x0, y0, k-0.0001), vec3(x1, y1, k+0.0001));
        return true;
    }
};

bool xy_rect::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
    float t = (k-r.origin().z()) / r.direction().z();
    if(t < tmin || t > tmax)
        return false;
    float x = r.origin().x() + t * r.direction().x();
    float y = r.origin().y() + t * r.direction().y();
    if( x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    rec.u = ( x - x0 ) / ( x1 - x0);
    rec.v = ( y - y0 ) / ( y1 - y0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0, 0, 1);
    return true;
}
#endif //RAY_TRACING_RECTANGLE_H
