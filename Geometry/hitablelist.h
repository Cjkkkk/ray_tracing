//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_HITABLELIST_H
#define RAY_TRACING_HITABLELIST_H

#include "hitable.h"

class hitable_list: public hitable  {
public:
    hitable_list() {}
    hitable_list(hitable **l, int n) {list = l; list_size = n; }
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec);
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    hitable **list;
    int list_size;
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    for (int i = 0; i < list_size; i++) {
        if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

bool hitable_list::bounding_box(float t0, float t1, aabb &box) const {
    // 如果size小于1一定发生了错误
    if(list_size < 1)return false;
    aabb temp_box; // 暂时的box
    // 获得第一个物体的box
    bool first_box_true = list[0]->bounding_box(t0, t1, temp_box);
    if(! first_box_true)return false;
    else
        box = temp_box;
    // 获取之后每一个物体的box 每得到1个就与之前的box做surrounding_box
    for(int i = 1 ; i < list_size ; i++){
        if(list[i]->bounding_box(t0, t1, temp_box)) {
            box = surrounding_box(box, temp_box);
        }
        else
            return false;
    }
    return true;
}

#endif //RAY_TRACING_HITABLELIST_H
