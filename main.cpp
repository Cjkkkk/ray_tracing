#include <iostream>
#include <limits>
#include <fstream>
#include <time.h>
#include "Geometry/sphere.h"
#include "Geometry/moving_sphere.h"
#include "Geometry/hitablelist.h"
#include "camera.h"
#include "Material/material.h"
#include "Material/texture.h"
#include "Utils/s_random.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"
#include "Geometry/rectangle.h"
#include "Geometry/flip.h"
#include "Geometry/box.h"
#include "Geometry/rotate_y.h"
#include "Geometry/translate.h"

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation*color(scattered, world, depth+1);
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
hitable *cornel_box(){
    hitable **list = new hitable*[8];
    int i = 0;
    material *red = new lambertian(new const_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new const_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new const_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new const_texture(vec3(15, 15, 15)));
    list[i++] = new flip(new yz_rect(0, 555, 0, 555, 555 ,green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip(new xy_rect(0, 555, 0, 555, 555, white));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0 , 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0 , 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
    return new hitable_list(list, i);
}
hitable *simple_light(){
    texture *pertex = new noise_texture(4);
    hitable **list = new hitable*[4];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian( pertex ));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertex));
    list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new const_texture(vec3(1, 1, 1))));
    list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new const_texture(vec3(1, 1, 1))));
    return new hitable_list(list, 4);
}
hitable* random_scene(){
    int n = 500;
    hitable **list = new hitable* [n+1];
    texture *checker = new checker_texture(new const_texture(vec3(0.2, 0.3, 0.1)), new const_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    int i = 1;
    for(int a = -11; a < 11 ; a++){
        for(int b = -11 ; b < 11 ; b++){
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(), 0.2, b+0.9*drand48());
            if((center - vec3(4,0.2,0)).length() > 0.9){
                if(choose_mat < 0.8){
                    list[i++] = new moving_sphere(center, center + vec3(0, 0.5*drand48(), 0), 0.0, 1.0, 0.2,
                            new lambertian(new const_texture(vec3(drand48()*drand48(),drand48()* drand48(), drand48()*drand48()))));
                }else if(choose_mat < 0.95){
                    list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1+drand48()), 0.5*(1+drand48()),0.5*(1+drand48())), 0.5*drand48()));
                }else{
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }
    list[i++] = new sphere(vec3(0,1,0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4,1,0), 1.0, new lambertian(new const_texture(vec3(0.4,0.2,0.1))));
    list[i++] = new sphere(vec3(4,1,0),1.0, new metal(vec3(0.7,0.6,0.6), 0.0));
    return new hitable_list(list, i);
}

hitable *earth() {
    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earthmap1k.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    return new sphere(vec3(0, 3, 0), 2, mat);
}

int main(int argc, char** argv) {
    if(argc != 2)std::cout << "please specify output filename" << std::endl;
    std::ofstream outfile;
    outfile.open(argv[1], std::ios::out);
    int nx = 800;
    int ny = 400;
    int ns = 20;
    outfile << "P3\n" << nx << " " << ny << "\n255\n";
    hitable* world = cornel_box();
    vec3 lookfrom = vec3(278, 278, -800);
    vec3 lookat = vec3(278, 278, 0);
    float dist_to_focus = 10;
    float aperture = 0;
    camera cam(lookfrom, lookat, vec3(0,1,0), 40, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    clock_t startTime,endTime;
    startTime = clock();

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = (i + drand48()) / float(nx);
                float v = (j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world,0);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            outfile << std::min(ir,255) << " " << std::min(ig,255) << " " << std::min(ib,255) << "\n";
        }
    }
    endTime = clock();
    std::cout << "Totle Time : " <<(double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;
    outfile.close();

}
