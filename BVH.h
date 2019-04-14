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
    std::vector<BVH*>* child;
};

void insertIntoBVHTree(BVH* root, hitable* obj, float time0, float time1){
    aabb box;
    obj->bounding_box(time0, time1, box); // 获取物体的bounding box
    auto curr = root;
    while(true) {
        if(!curr->child) {
            // 叶子
            curr->child = new std::vector<BVH*>(8);
            // 分裂8个slab
            auto index = curr->box->getSubSlotIndex(box.center()); // 判断物体在box的8个slab中的位置
            curr->child->at(index) = new BVH(nullptr, obj, time0, time1);
            auto subBox = curr->box->getSubSlot(index);
            curr->child->at(index)->box = new aabb(subBox->min(), subBox->max());
            // 下移叶子节点中的元素
            insertIntoBVHTree(curr, curr->obj, time0, time1);
            break;
        }else {
            // 不是叶子的情况
            auto index = curr->box->getSubSlotIndex(box.center()); // 判断物体在box的8个slab中的位置
            // 子女是叶子 直接插入
            if(!curr->child->at(index)){
                curr->child->at(index) = new BVH(nullptr, obj, time0, time1);
                auto subBox = curr->box->getSubSlot(index);
                curr->child->at(index)->box = new aabb(subBox->min(), subBox->max());
                break;
            }else{
                curr = curr->child->at(index);
            }
        }
    }
}
void bottomUpBounding(BVH* root, float time0, float time1){
    if(!root->child) {
        root->obj->bounding_box(time0, time1, *root->box);
    } else {
        for(int i = 0 ; i < 8; i ++){
            if(root->child->at(i)){
                bottomUpBounding(root->child->at(i),time0, time1);
                *root->box = surrounding_box(*root->box, *root->child->at(i)->box);
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
    bottomUpBounding(root, time0, time1);
//    std::cout << box1.min() << std::endl;
//    std::cout << box1.max() << std::endl;
    return root;
}

bool BVH::bounding_box(float t0, float t1, aabb& b) const {
    b = *box;
    return true;
}

bool BVH::hit(const ray &r, float t_min, float t_max, hit_record &rec) {
    if(!child) {
        //到达叶子节点了
        if(obj->hit(r, t_min, t_max, rec)) {
            return true;
        }else{
            return false;
        }
    }
    if(box->hit(r, t_min, t_max)) {
        bool result = false;
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
