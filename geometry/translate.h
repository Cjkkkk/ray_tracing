//
// Created by DELL on 2019/2/28.
//

#ifndef RAY_TRACING_TRANSLATE_H
#define RAY_TRACING_TRANSLATE_H

#include "hitable.h"

//class translate: public hitable{
//public:
//    translate(hitable* _ptr, const vec3& _offset):ptr(_ptr), offset(_offset){}
//    virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
//    virtual bool bounding_box(float t0, float t1, aabb& cornell_box) const {
//        if(ptr->bounding_box(t0, t1, cornell_box)){
//            cornell_box._min += offset;
//            cornell_box._max += offset;
//            return true;
//        }
//        return false;
//
//    }
//    hitable* ptr;
//    vec3 offset;
//};
//
//bool translate::hit(const ray &r, float t0, float t1, hit_record &rec) const {
//    ray reverse_trans_ray = ray(r.origin() - offset, r.origin());
//    if(ptr->hit(reverse_trans_ray, t0, t1, rec)){
//        rec.p += offset;
//        return true;
//    }
//    return false;
//}


class translate : public hitable {
public:
    translate(hitable *p, const vec3& displacement) : ptr(p), offset(displacement) {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec);
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    hitable *ptr;
    vec3 offset;
};

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (ptr->hit(moved_r, t_min, t_max, rec)) {
        rec.p += offset;
        return true;
    }
    else
        return false;
}

bool translate::bounding_box(float t0, float t1, aabb& box) const {
    if (ptr->bounding_box(t0, t1, box)) {
        box = aabb(box.min() + offset, box.max()+offset);
        return true;
    }
    else
        return false;
}
#endif //RAY_TRACING_TRANSLATE_H
