#include <iostream>
#include <limits>
#include <fstream>
#include <ctime>
#include <thread>
#include <future>
#include "geometry/hitable.h"
#include "geometry/sphere.h"
#include "geometry/moving_sphere.h"
#include "geometry/hitablelist.h"
#include "camera.h"
#include "material/material.h"
#include "material/texture.h"
#include "utils/s_random.h"
#include "geometry/rectangle.h"
#include "geometry/flip.h"
#include "geometry/cornell_box.h"
#include "geometry/rotate_y.h"
#include "geometry/translate.h"
#include "geometry/triangle.h"

#include <cuda_runtime.h>
#define checkCudaErrors(func)				\
{									\
    cudaError_t e = (func);			\
    if(e != cudaSuccess)						                \
        printf ("%s %d CUDA: %s\n", __FILE__,  __LINE__, cudaGetErrorString(e));		\
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
}

__host__ __device__ 
vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001f, std::numeric_limits<float>::max(), rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 20 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
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

__global__ void intersect(vec3* pixel, camera* cam, hitable* world, int ny, int nx, int ns)
{
    int bx = blockIdx.x;
    int by = blockIdx.y;

    int bszx = blockDim.x;
    int bszy = blockDim.y;
    // Thread index
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    int i = bszy * by + ty;
    int j = bszx * bx + tx;

    vec3 col(0, 0, 0);
    for (int s = 0; s < ns; s++) {
        float u = (i + drand48()) / float(nx);
        float v = (j + drand48()) / float(ny);
        ray r = cam->get_ray(u, v);
        col += color(r, world,0);
    }
    col /= float(ns);
    col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
    pixel[j * nx + i] = vec3{
        fminf(255.99f*col[0],255.0f), 
        fminf(255.99f*col[1],255.0f), 
        fminf(255.99f*col[2],255.0f)
    };
}

int main(int argc, char** argv) {
    if(argc != 2) std::cout << "please specify output filename" << std::endl;
    std::ofstream outfile;
    outfile.open(argv[1], std::ios::out);
    const int nx = 256;
    const int ny = 256;
    int ns = 20;
    std::vector<std::vector<std::vector<float>>> res(ny);
    for(int i = 0 ; i < ny ; i ++){
        res[i] = std::vector<std::vector<float>>(nx);
        for(int j = 0 ; j < nx ; j ++){
            res[i][j] = std::vector<float>(3);
        }
    }
    outfile << "P3\n" << nx << " " << ny << "\n255\n";

    hitable* world = cornel_box_();
    vec3 lookfrom = vec3(278, 278, -800);
    vec3 lookat = vec3(278, 278, 0);

    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0;
    camera* cam = new camera(lookfrom, lookat, vec3(0,1,0), 60, float(nx) / float(ny), aperture, dist_to_focus, 0.0, 1.0);

    vec3* pixel = new vec3[ny * nx];
    
    cudaEvent_t start, stop;
    float msecTotal = 0;
    checkCudaErrors(cudaEventCreate(&start));
    checkCudaErrors(cudaEventCreate(&stop));

    dim3 dimBlock(32, ny / 32);
    dim3 dimGrid(nx / 32, ny / 32);
    intersect<<< dimGrid, dimBlock >>>(pixel, cam, world, ny, nx, ns);

    checkCudaErrors(cudaEventRecord(stop));
    checkCudaErrors(cudaEventSynchronize(stop));
    checkCudaErrors(cudaEventElapsedTime(&msecTotal, start, stop));
    std::cout << "Total Time : " << msecTotal / 1000 / CLOCKS_PER_SEC << "s" << std::endl;

    for (int j = ny - 1 ; j >= 0 ; j --)
        for ( int i = 0 ; i < nx ; i ++ )
            outfile << static_cast<int>(pixel[j * nx + i].x()) 
            << " " << static_cast<int>(pixel[j * nx + i].y()) 
            << " " << static_cast<int>(pixel[j * nx + i].z()) << "\n";

    outfile.close();
}
