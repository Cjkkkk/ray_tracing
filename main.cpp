#include <iostream>
#include <limits>
#include <fstream>
#include <ctime>
#include <thread>
#include <future>
#if defined(__MINGW32__) && defined(__GNUC__)
#include "include/mingw.thread.h"
#include "include/mingw.mutex.h"
#endif
#include "Geometry/hitable.h"
#include "Geometry/sphere.h"
#include "Geometry/moving_sphere.h"
#include "Geometry/hitablelist.h"
#include "camera.h"
#include "Material/material.h"
#include "Material/texture.h"
#include "Utils/s_random.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"
#include "BVH.h"
#include "Geometry/rectangle.h"
#include "Geometry/flip.h"
#include "Geometry/cornell_box.h"
#include "Geometry/rotate_y.h"
#include "Geometry/translate.h"
#include "Geometry/triangle.h"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 20 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            //std::cout << attenuation << "\n";
            return emitted + attenuation * color(scattered, world, depth+1);
            // 一般物体材质不发光 第一项为0 发光物体不散射 返回亮度
        }
        else {
            return emitted;
        }
    }
    else {
        return vec3(0, 0, 0); // 环境光
    }
}
hitable *cornel_box_(){
    hitable **list = new hitable*[8];
    int i = 0;
    material *red = new lambertian(new const_texture(vec3(0.65f, 0.05f, 0.05f)));
    material *white = new lambertian(new const_texture(vec3(0.73f, 0.73f, 0.73f)));
    material *green = new lambertian(new const_texture(vec3(0.12f, 0.45f, 0.15f)));
    material *light = new diffuse_light(new const_texture(vec3(15, 15, 15)));
    list[i++] = new flip(new yz_rect(0, 555, 0, 555, 555 ,green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(new rotate_y(new cornell_box(vec3(0, 0 , 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
    list[i++] = new translate(new rotate_y(new cornell_box(vec3(0, 0 , 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
    return new hitable_list(list, i);
    //return getBVHHierarchy(list, 8, 0.001, FLT_MAX);
}
hitable *simple_light(){
    texture *pertex = new noise_texture(4);
    hitable **list = new hitable*[4];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian( pertex ));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertex));
    list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new const_texture(vec3(1, 1, 1))));
    list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new const_texture(vec3(1, 1, 1))));
    //return getBVHHierarchy(list, 4, 0.001, FLT_MAX);
    return new hitable_list(list, 4);
}
hitable* random_scene(){
    int n = 486;
    hitable **list = new hitable* [n];
    texture *checker = new checker_texture(new const_texture(vec3(0.2f, 0.3f, 0.1f)), new const_texture(vec3(0.9f, 0.9f, 0.9f)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for(int a = -11; a < 11 ; a++){
        for(int b = -11 ; b < 11 ; b++){
            float choose_mat = drand48();
            vec3 center(a + 0.9f * drand48(), 0.2f , b + 0.9f * drand48());
            if((center - vec3(4, 0.2f, 0)).length() > 0.9f){
                if(choose_mat < 0.8){
                    //list[i++] = new moving_sphere(center, center + vec3(0, 0.5*drand48(), 0), 0.0, 1.0, 0.2,
                            //new lambertian(new const_texture(vec3(drand48()*drand48(),drand48()* drand48(), drand48()*drand48()))));
                    list[i++] = new sphere(center, 0.2f, new lambertian(new const_texture(vec3(drand48()*drand48(),drand48()* drand48(), drand48()*drand48()))));
                }else if(choose_mat < 0.95f){
                    list[i++] = new sphere(center, 0.2f, new metal(vec3(0.5f*(1+drand48()), 0.5f*(1+drand48()),0.5f*(1+drand48())), 0.5f*drand48()));
                }else{
                    list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0f, new lambertian(new const_texture(vec3(0.4f, 0.2f, 0.1f))));
    list[i++] = new sphere(vec3(4, 1, 0),1.0f, new metal(vec3(0.7f, 0.6f, 0.6f), 0.0f));
    return getBVHHierarchy(list, n, 0.001f, FLT_MAX);
    return new hitable_list(list, i);
}

hitable *earth() {
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earthmap1k.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    return new sphere(vec3(0, 3, 0), 2, mat);
}

hitable *brdf_() {
    hitable **list = new hitable* [3];
    auto text = new const_texture(vec3(0.5f, 0.8f, 0.7f));
    auto mat = new brdf(text, 0.04f, 0.35f);
    list[0] = new sphere(vec3(0, 5, 0), 4, mat);
    list[1] = new sphere(vec3(10, 5, 0), 4, new lambertian(text));
    list[2] = new sphere(vec3(5, 15, 5), 5, new diffuse_light(new const_texture(vec3(1,1,1))));
    return new hitable_list(list, 3);
}

hitable *simple_triangle(){
    hitable **list = new hitable*[2];
    list[0] = new triangle(vec3(-2, 0, 0), vec3(2, 0, 0), vec3(0, 2, 0), new diffuse_light(new const_texture(vec3(0, 1, 0))));
    list[1] = new triangle(vec3(-1, 1, 1), vec3(3, 1, 1), vec3(1, 3, 1), new diffuse_light(new const_texture(vec3(1, 0, 0))));
    return getBVHHierarchy(list, 2, 0.001f, FLT_MAX);
}

std::atomic<int> hitable::intersection_times = {0};

//void parallel_ray_tracing_computation(
//        int ny_start, int ny_end, int nx_start, int nx_end,
//        int ns, camera& cam, hitable* world, std::vector<std::vector<std::vector<float>>>& res) {
//    auto ny = res.size();
//    auto nx = res[0].size();
//    for (int j = ny_start; j < ny_end; j++) {
//        for (int i = nx_start; i < nx_end; i++) {
//            vec3 col(0, 0, 0);
//            for (int s=0; s < ns; s++) {
//                float u = (i + drand48()) / float(nx);
//                float v = (j + drand48()) / float(ny);
//                ray r = cam.get_ray(u, v);
//                vec3 p = r.point_at_parameter(2.0);
//                col += color(r, world,0);
//            }
//            col /= float(ns);
//            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
//            res[j][i][0] = (std::min(int(255.99f * col[0]),255));
//            res[j][i][1] = (std::min(int(255.99f * col[1]),255));
//            res[j][i][2] = (std::min(int(255.99f * col[2]),255));
//        }
//    }
//}
int main(int argc, char** argv) {
    if(argc != 2)std::cout << "please specify output filename" << std::endl;
    std::ofstream outfile;
    outfile.open(argv[1], std::ios::out);
    const int nx = 500;
    const int ny = 250;
    int ns = 200;
    std::vector<std::vector<std::vector<float>>> res(ny);
    for(int i = 0 ; i < ny ; i ++){
        res[i] = std::vector<std::vector<float>>(nx);
        for(int j = 0 ; j < nx ; j ++){
            res[i][j] = std::vector<float>(3);
        }
    }
    outfile << "P3\n" << nx << " " << ny << "\n255\n";
//    hitable* world = random_scene();
//    vec3 lookfrom = vec3(0, 0, 10);
//    vec3 lookat = vec3(-5, 0, 1);
//    vec3 lookfrom = vec3(13, 2, 3);
//    vec3 lookat = vec3(0, 0, 0);
    hitable* world = cornel_box_();
    vec3 lookfrom = vec3(278, 278, -800);
    vec3 lookat = vec3(278, 278, 0);
//    hitable* world = brdf_();
//    vec3 lookfrom = vec3(5, 5, 20);
//    vec3 lookat = vec3(5, 5, 0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0;
    camera cam(lookfrom, lookat, vec3(0,1,0), 60, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);
    clock_t startTime,endTime;
    startTime = clock();

    std::vector<std::vector<vec3>> pixel(
            ny,std::vector<vec3>(nx));
    std::size_t cores = std::thread::hardware_concurrency();
    std::vector<std::future<void>> future_vector;
    for (std::size_t core(0); core < cores; ++core) {
        future_vector.emplace_back(std::async([=, &cam, &pixel]() {
            for (int j = core; j < ny; j+=cores) {
                for (int i = 0; i < nx; i+=1) {
                    vec3 col(0, 0, 0);
                    for (int s=0; s < ns; s++) {
                        float u = (i + drand48()) / float(nx);
                        float v = (j + drand48()) / float(ny);
                        ray r = cam.get_ray(u, v);
//                        vec3 p = r.point_at_parameter(2.0);
                        col += color(r, world,0);
                    }
                    col /= float(ns);
                    col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
//                    int ir = int(255.99*col[0]);
//                    int ig = int(255.99*col[1]);
//                    int ib = int(255.99*col[2]);
                    pixel[j][i] = vec3{std::min(255.99f*col[0],255.0f), std::min(255.99f*col[1],255.0f), std::min(255.99f*col[2],255.0f)};
                }
            }
        }));
    }
    for (auto &f : future_vector) {
        f.get();
    }
    for (int j = ny - 1 ; j >= 0 ; j --)
        for ( int i = 0 ; i < nx ; i ++ )
            outfile << static_cast<int>(pixel[j][i].x()) << " " << static_cast<int>(pixel[j][i].y()) << " " << static_cast<int>(pixel[j][i].z()) << "\n";

    endTime = clock();
    std::cout << "Total Time : " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;
    std::cout << "Total number of intersection tests: " << hitable::intersection_times << std::endl;
    outfile.close();
}
