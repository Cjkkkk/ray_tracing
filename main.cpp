#include <iostream>
#include <limits>
#include "Geometry/sphere.h"
#include "Geometry/moving_sphere.h"
#include "Geometry/hitablelist.h"
#include "camera.h"
#include "Material/material.h"
#include "Material/texture.h"
#include "s_random.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Material/stb_image.h"
#include "Material/diffuse_light.h"
#include "Geometry/rectangle.h"
vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation*color(scattered, world, depth+1);
        }
        else {
            return emitted;
        }
    }
    else {
        return vec3(0, 0, 0);
    }
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
    //unsigned char *tex_data = stbi_load("tiled.jpg", &nx, &ny, &nn, 0);
    unsigned char *tex_data = stbi_load("earthmap1k.jpg", &nx, &ny, &nn, 0);
    material *mat =  new lambertian(new image_texture(tex_data, nx, ny));
    return new sphere(vec3(0, 3, 0), 2, mat);
}

int main() {
    int nx = 2000;
    int ny = 1000;
    int ns = 10;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
//    hitable *list[4];
//    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
//    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
//    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    //list[3] = new sphere(vec3(-1,0,-1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.0));
//    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    //hitable *world = new hitable_list(list,4);
    //hitable* world = random_scene();
//    hitable *list[2];
//    texture *checker = new checker_texture(new const_texture(vec3(0, 0, 0)), new const_texture(vec3(1, 1, 1)));
//    texture *noise_t = new noise_texture();
//    list[0] = new sphere(vec3(0, 4, 0), 1, new diffuse_light(new const_texture(vec3(1, 1, 1))));
//    list[1] = new sphere(vec3(0, -100, 0), 100, new lambertian(new const_texture(vec3(0.5, 0.6, 0.4))));
    hitable* world = simple_light();
    vec3 lookfrom = vec3(0, 12, 13);
    vec3 lookat = vec3(0, 3, 0);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0;
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 0.0);

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world,0);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
