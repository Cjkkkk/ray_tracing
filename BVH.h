//
// Created by kk on 2/25/19.
//

#ifndef RAY_TRACING_BVH_NODE_H
#define RAY_TRACING_BVH_NODE_H

#include <vector>
#include <queue>
#include "Geometry/hitable.h"
#include "Geometry/hitablelist.h"
#include "Geometry/triangle.h"

class BVH : public hitable {
public:
    BVH(aabb box_, uint8_t depth_): box(box_), depth(depth_), isLeaf(true), child(std::vector<BVH*>(8)){}
    ~BVH(){
        for(auto ptr: child) delete(ptr);
    }
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) override ;
    bool bounding_box(float t0, float t1, aabb& box) const override;
    aabb box;
    bool isLeaf;
    uint8_t depth;
    float t;
    std::vector<hitable*> data;
    std::vector<BVH*> child;
    bool operator < (const BVH*a) const
    {
        //由小到大排列，如果要由大到小排列，使用“>”即可；
        return a->t < t;
    }
};

void insertIntoBVHTree(BVH* root, hitable* obj, float time0, float time1, uint8_t depth){
    auto curr = root;
    if(curr->isLeaf) {
        // 叶子
        if(curr->data.size() == 0 || depth == 16) {
            curr->data.push_back(obj);
        } else{
            curr->isLeaf = false;
            while(curr->data.size()){
                insertIntoBVHTree(curr, curr->data.back(), time0, time1, depth);
                curr->data.pop_back();
            }
            insertIntoBVHTree(curr, obj, time0, time1, depth);
        }
    }else {
        // 不是叶子的情况
        aabb box;
        obj->bounding_box(time0, time1, box); // 获取物体的bounding box
        auto index = curr->box.getSubSlotIndex(box.center()); // 判断物体在box的8个slab中的位置
        // 子女是叶子 直接插入
        if(!curr->child[index]) {
            auto subBox = curr->box.getSubSlot(index);
            curr->child[index] = new BVH(aabb(subBox->min(), subBox->max()), depth + 1);
        }
        insertIntoBVHTree(curr->child[index], obj, time0, time1, depth + 1);
    }
}

void bottomUpBounding(BVH* root, float time0, float time1){
    if(root->isLeaf) {
        aabb temp_box;
        for(auto obj : root->data){
            obj->bounding_box(time0, time1, temp_box);
            root->box = surrounding_box(temp_box, root->box);
        }
    } else {
        for(int i = 0 ; i < 8; i ++){
            if(root->child[i]){
                bottomUpBounding(root->child[i],time0, time1);
                root->box = surrounding_box(root->box, root->child[i]->box);
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
    auto root = new BVH(box1, 0);
    for(int i = 0 ; i < n ; i++) {
        insertIntoBVHTree(root, l[i], time0, time1, 0);
    }
    bottomUpBounding(root, time0, time1);
    return root;
}

bool BVH::bounding_box(float t0, float t1, aabb& b) const {
    b = box;
    return true;
}

bool BVH::hit(const ray &r, float t_min, float t_max, hit_record &rec) {
    std::priority_queue <BVH*> pq;
    bool ans = false;
    if(this->box.hit(r, t_min, t_max, rec)) {
        this->t = rec.t;
        pq.push(this);
    }
    while(pq.size() && pq.top()->t < t_max) {
        auto item = pq.top();
        pq.pop();
        if(item->isLeaf) {
            for(auto obj : item->data) {
                if(obj->hit(r, t_min, t_max, rec)){
                    t_max = rec.t;
                    ans = true;
                }
            }
        } else {
            for(auto child: item->child) {
                if(child && child->box.hit(r, t_min, t_max, rec)) {
                    child->t = rec.t;
                    pq.push(child);
                }
            }
        }
    }
    return ans;
}


#endif //RAY_TRACING_BVH_NODE_H
