//
// Created by DELL on 2018/11/30.
//

#ifndef RAY_TRACING_HITABLE_H
#define RAY_TRACING_HITABLE_H

#include "../ray.h"

class material;

struct hit_record
{
    float t;
    float u;
    float v;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
};

inline float ffmin(float a, float b) { return a < b ? a : b;}
inline float ffmax(float a, float b) { return a > b ? a : b;}

class aabb {
public:
    aabb(){}
    aabb(const vec3& a, const vec3& b) : _min(a), _max(b) {
        _center = vec3((a.x() + b.x()) / 2,
                (a.y() + b.y()) / 2,
                (a.z() + b.z()) / 2);
    }
    int getSubSlotIndex(const vec3& pos) const {
        bool back = pos.z() > _center.z();
        bool left = pos.x() > _center.x();
        bool up = pos.y() < _center.y();
        return up * 4 + back * 2 + left * 1;
    }
    aabb* getSubSlot(const int index){
        vec3 pMin, pMax;
        pMin[0] = (index & 4) ? center()[0] : min()[0];
        pMax[0] = (index & 4) ? max()[0] : center()[0];
        pMin[1] = (index & 2) ? center()[1] : min()[1];
        pMax[1] = (index & 2) ? max()[1] : center()[1];
        pMin[2] = (index & 1) ? center()[2] : min()[2];
        pMax[2] = (index & 1) ? max()[2] : center()[2];
        return new aabb(pMin, pMax);
//        if(index == 0){
//            auto pMin = vec3(min().x(), center().y(), center().z());
//            auto pMax = vec3(center().x(),max().y(), max().z());
//            box = new aabb(pMin, pMax);
//        }else if(index == 1) {
//            auto pMin = vec3(center().x(), center().y(), center().z());
//            auto pMax = vec3(max().x(),max().y(), max().z());
//            box = new aabb(pMin, pMax);
//        }else if(index == 2){
//            auto pMin = vec3(min().x(), center().y(), min().z());
//            auto pMax = vec3(center().x(), max().y(), center().z());
//            box = new aabb(pMin, pMax);
//        }else if(index == 3){
//            auto pMin = vec3(center().x(), center().y(), min().z());
//            auto pMax = vec3(max().x(),max().y(), center().z());
//            box = new aabb(pMin, pMax);
//        }else if(index == 4){
//            auto pMin = vec3(min().x(),min().y(),center().z());
//            auto pMax = vec3(center().x(), center().y(), max().z());
//            box = new aabb(pMin, pMax);
//        }else if(index == 5){
//            auto pMin = vec3(center().x(),min().y(),center().z());
//            auto pMax = vec3(max().x(), center().y(), max().z());
//            box = new aabb(pMin, pMax);
//        }else if(index == 6){
//            auto pMin = vec3(min().x(), min().y(), min().z());
//            auto pMax = vec3(center().x(),center().y(), center().z());
//            box = new aabb(pMin, pMax);
//        }else if(index == 7){
//            auto pMin = vec3(center().x(), min().y(), min().z());
//            auto pMax = vec3(max().x(),center().y(), center().z());
//            box = new aabb(pMin, pMax);
//        }else{
//            std::cout << "error!" << std::endl;
//        }
//        return box;
    }
    vec3 max() { return _max; }
    vec3 min() { return _min; }
    vec3 center() { return _center;}
    bool hit(const ray& r, float tmin, float tmax) const {
        for(int a = 0 ; a < 3 ; a++){
            float t0 = ffmin(
                    (_min[a] - r.origin()[a]) / r.direction()[a],
                    (_max[a] - r.origin()[a]) / r.direction()[a]
            );
            float t1 = ffmax(
                    (_min[a] - r.origin()[a]) / r.direction()[a],
                    (_max[a] - r.origin()[a]) / r.direction()[a]
            );

            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            if(tmax <= tmin) return false;
        }
        return true;
    }
    vec3 _min;
    vec3 _max;
    vec3 _center;
};

aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small(
            ffmin(box0.min().x(), box1.min().x()),
            ffmin(box0.min().y(), box1.min().y()),
            ffmin(box0.min().z(), box1.min().z()));
    vec3 big(
            ffmax(box0.max().x(), box1.max().x()),
            ffmax(box0.max().y(), box1.max().y()),
            ffmax(box0.max().z(), box1.max().z()));
    return aabb(small, big);
}

class hitable  {
public:
    static int intersection_times;
    void increaseIntersectionTimes(){
        intersection_times += 1;
    }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0; // 获得物体的bounding cornel_box
//    int number_of_ray_object_test = 0 ;
};


#endif //RAY_TRACING_HITABLE_H
