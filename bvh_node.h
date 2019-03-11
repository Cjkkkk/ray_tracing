//
// Created by kk on 2/25/19.
//

#ifndef RAY_TRACING_BVH_NODE_H
#define RAY_TRACING_BVH_NODE_H

#include "Geometry/hitable.h"
#include "Geometry/hitablelist.h"

class bvh_node : public hitable {
public:
    bvh_node(hitable* left_, hitable* right_, float time0, float time1){
        left = left_;
        right = right_;
        aabb box1, box2;
        bool result1 = left->bounding_box(time0, time1, box1);
        bool result2 = right->bounding_box(time0, time1, box2);
        if(!result1 || ! result2)std::cout << "can not surr!" << std::endl;
        box = surrounding_box(box1, box2);
    }
//    bvh_node(hitable **l, int n, float time0, float time1){
//
//    }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec);
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
//    int cal_total_number_of_ray_object_tests();
    hitable *left;
    hitable *right;
    aabb box;
};



hitable* get_bvh_hier(hitable** l, int height, int n, float time0, float time1){
    int number_of_leaf = int(pow(2,  height ));
    bvh_node** bl = new bvh_node*[int(pow(2,  height -1))];
    int j = 0;
    if(n <= number_of_leaf) return new hitable_list(l, n);
    else{
        int hitable_number_in_small_leaf = n / number_of_leaf;
        int hitable_number_in_big_leaf = hitable_number_in_small_leaf + 1;
        int number_of_small_leaf = hitable_number_in_big_leaf * number_of_leaf - n;
        int offset = 0;
        for(int i = 0 ; i < number_of_leaf ; i+=2){
            int hitable_number_in_left_leaf = (i < number_of_small_leaf? hitable_number_in_small_leaf:hitable_number_in_big_leaf);
            int hitable_number_in_right_leaf = (i < number_of_small_leaf? hitable_number_in_small_leaf:hitable_number_in_big_leaf);
            //std::cout << i << " " << offset <<" "<< " " << hitable_number_in_left_leaf << " " << hitable_number_in_right_leaf << std::endl;
            hitable* lef = new hitable_list(&(l[offset]),hitable_number_in_left_leaf);
            offset += hitable_number_in_left_leaf;
            hitable* righ = new hitable_list(&(l[offset]), hitable_number_in_right_leaf);
            offset += hitable_number_in_right_leaf;
            bl[j++] = new bvh_node(lef, righ, time0, time1);
        }
    }
    while(j != 1){
        int current = 0;
        for(int i = 0 ; i < j ; i+=2){
            bl[current++] = new bvh_node(bl[i], bl[i+1], time0, time1);
        }
        j /= 2;
    }
    return bl[0];

}
bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
    b = box;
    return true;
}

bool bvh_node::hit(const ray &r, float t_min, float t_max, hit_record &rec) {
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
