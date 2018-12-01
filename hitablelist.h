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
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    hitable **list;
    int list_size;
};
/* 一系列物体
如果碰到任何物体则更新碰到的最近的物体
返回是否碰到任何物体
碰撞记录通过引用返回*/

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
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


#endif //RAY_TRACING_HITABLELIST_H
