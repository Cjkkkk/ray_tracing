//
// Created by kk on 2/25/19.
//

#ifndef RAY_TRACING_BVH_NODE_H
#define RAY_TRACING_BVH_NODE_H

#include "hitable.h"

class bvh_node : public hitable {
public:
    bvh_node(){}
    bvh_node(hitable **l, int n, float time0, float time1);
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    hitable *left;
    hitable *right;
    aabb box;
};

bool bvh_node::bounding_box(float t0, float t1, aabb &b) const {
    b = box;
    return true;
}

bool bvh_node ::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
    if(box.hit(r, t_min, t_max)) {
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if( hit_left && hit_right ) {
            if(left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        }else if(hit_left) {
            rec = left_rec;
            return true;
        }else if(hit_right) {
            rec = right_rec;
            return true;
        }else
            return false;
    }
    return false;
}
#endif //RAY_TRACING_BVH_NODE_H
