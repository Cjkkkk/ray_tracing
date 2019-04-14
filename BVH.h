//
// Created by kk on 2/25/19.
//

#ifndef RAY_TRACING_BVH_NODE_H
#define RAY_TRACING_BVH_NODE_H

#include <vector>
#include "Geometry/hitable.h"
#include "Geometry/hitablelist.h"

class BVH : public hitable {
public:
    BVH(aabb* box_, hitable* obj_, float time0, float time1){
        box = box_;
        obj = obj_;
        child = nullptr;
        state = false;
    }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec);
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    aabb* box;
    hitable* obj;
    bool state;
    std::vector<BVH*>* child;
};

void insertIntoBVHTree(BVH* root, hitable* obj, float time0, float time1){
    aabb box;
    obj->bounding_box(time0, time1, box);
    auto curr = root;
    auto index = curr->box->getSubSlotIndex(box.center());
    while(true) {
        if(!curr->child) {
            // 叶子
            curr->child = new std::vector<BVH*>(8);
            auto subBox = curr->box->getSubSlot(index);
            curr->child->at(index) = new BVH(nullptr, obj, time0, time1);
            curr->child->at(index)->box = new aabb(subBox->min(), subBox->max());
            break;
        }else if(!curr->child->at(index)) {
            curr->child->at(index) = new BVH(nullptr, obj, time0, time1);
            curr->child->at(index)->box = new aabb(box.min(), box.max());
            break;
        }else{
            curr = curr->child->at(index);
            if(!curr->child) {
                //叶子， 下移这个节点的元素
                auto temp = curr->obj;
//                curr->obj = nullptr;
                insertIntoBVHTree(curr, temp, time0, time1);
            }
        }
    }
}
hitable* getBVHHierarchy(hitable** l, int n, float time0, float time1){
    aabb box1, box2;
    l[0]->bounding_box(time0, time1, box1);
    for(int i = 1; i < n ; i++){
        l[i]->bounding_box(time0, time1, box2);
        box1 = surrounding_box(box1, box2);
    }
    auto root = new BVH(nullptr, l[0], time0, time1);
    root->box = new aabb(box1.min(), box1.max());
    for(int i = 1 ; i < n ; i++) {
        insertIntoBVHTree(root, l[i], time0, time1);
        //std::cout << box1.center() << " | " << box2.center() << " "<< "index: " << box1.getSubSlotIndex(box2._center) << std::endl;
    }
//    std::cout << box1.min() << std::endl;
//    std::cout << box1.max() << std::endl;
    return root;
}


//hitable* get_bvh_hier(hitable** l, int height, int n, float time0, float time1){
//    int number_of_leaf = int(pow(2,  height ));
//    BVH** bl = new BVH*[int(pow(2,  height -1))];
//    int j = 0;
//    if(n <= number_of_leaf) return new hitable_list(l, n);
//    else{
//        int hitable_number_in_small_leaf = n / number_of_leaf;
//        int hitable_number_in_big_leaf = hitable_number_in_small_leaf + 1;
//        int number_of_small_leaf = hitable_number_in_big_leaf * number_of_leaf - n;
//        int offset = 0;
//        for(int i = 0 ; i < number_of_leaf ; i+=2){
//            int hitable_number_in_left_leaf = (i < number_of_small_leaf? hitable_number_in_small_leaf:hitable_number_in_big_leaf);
//            int hitable_number_in_right_leaf = (i < number_of_small_leaf? hitable_number_in_small_leaf:hitable_number_in_big_leaf);
//            //std::cout << i << " " << offset <<" "<< " " << hitable_number_in_left_leaf << " " << hitable_number_in_right_leaf << std::endl;
//            hitable* lef = new hitable_list(&(l[offset]),hitable_number_in_left_leaf);
//            offset += hitable_number_in_left_leaf;
//            hitable* righ = new hitable_list(&(l[offset]), hitable_number_in_right_leaf);
//            offset += hitable_number_in_right_leaf;
//            bl[j++] = new BVH(lef, righ, time0, time1);
//        }
//    }
//    while(j != 1){
//        int current = 0;
//        for(int i = 0 ; i < j ; i+=2){
//            bl[current++] = new BVH(bl[i], bl[i+1], time0, time1);
//        }
//        j /= 2;
//    }
//    return bl[0];
//
//}
bool BVH::bounding_box(float t0, float t1, aabb& b) const {
    b = *box;
    return true;
}

bool BVH::hit(const ray &r, float t_min, float t_max, hit_record &rec) {
    if(box->hit(r, t_min, t_max)) {
        hit_record rec;
        bool result = false;
        if(!child) {
            //到达叶子节点了
            if(obj->hit(r, t_min, t_max, rec)) {
                return true;
            }else{
                return false;
            }
        }
        // 递归检查子节点
        for(int i = 0 ; i < child->size() ; i++){
            if(child->at(i)) {
                result = child->at(i)->hit(r, t_min, t_max, rec);
                if(result) {
                    t_max = rec.t;
                }
            }
        }
        if(result) return true;
    }
    return false;
}


#endif //RAY_TRACING_BVH_NODE_H
