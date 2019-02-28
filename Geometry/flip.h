//
// Created by DELL on 2019/2/28.
//

#ifndef RAY_TRACING_FLIP_H
#define RAY_TRACING_FLIP_H

#include "hitable.h"

class flip: public hitable{
public:
    flip(hitable* _h): h(_h){}
    virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb &box) const;
    hitable* h;
};

bool flip::hit(const ray &r, float t0, float t1, hit_record &rec) const {
    if(h->hit(r, t0, t1, rec)){
        rec.normal = -rec.normal;
        return true;
    };
    return false;
}
bool flip::bounding_box(float t0, float t1, aabb &box) const {
    return h->bounding_box(t0, t1, box);
}
#endif //RAY_TRACING_FLIP_H
