//
// Created by DELL on 2018/12/1.
//

#ifndef RAY_TRACING_CAMERA_H
#define RAY_TRACING_CAMERA_H

#include <stdlib.h>
#include "ray.h"
// #include "utils/s_random.h"

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

CUDA_CALLABLE_MEMBER
vec3 random_in_unit_disk(){
    vec3 p;
    do {
        p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    }while(dot(p, p) >= 1.0);
    return p;
}

class camera {
public:
    CUDA_CALLABLE_MEMBER camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vfov, float aspect, float aperture, float focus_dist, float t0, float t1){
        time0 = t0;
        time1 = t1;
        lens_radius = aperture / 2; //半径是光圈大小的1/2
        float theta = vfov * M_PI / 180; //弧度制
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height; //长宽比
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w,u);

        lower_left_corner = vec3(-half_width, -half_height, -1.0);
        lower_left_corner = origin + (- half_width * u * focus_dist - half_height * v * focus_dist- w * focus_dist);
        horizontal = 2 * u * half_width * focus_dist;
        vertical = 2 * v * half_height * focus_dist;
    }
    CUDA_CALLABLE_MEMBER ray get_ray(float s, float t){
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        float time = time0 + drand48() * (time1 - time0);
        return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, time);
    }
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u,v,w;
    float lens_radius;
    float time0, time1;

};


#endif //RAY_TRACING_CAMERA_H
