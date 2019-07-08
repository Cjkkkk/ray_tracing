//
// Created by kk on 2/25/19.
//

#ifndef RAY_TRACING_TEXTURE_H
#define RAY_TRACING_TEXTURE_H

#include "../vec3.h"
#include "perlin.h"
class texture {
public:
    virtual vec3 value(float u, float v, const vec3 & p) const  = 0;
};

class const_texture : public texture {
public:
    const_texture(){}
    const_texture(vec3 c) : color(c) {}
    virtual vec3 value(float u, float v, const vec3& p)const {
        return color;
    }
    vec3 color;
};

class checker_texture : public texture {
public:
    checker_texture() {}
    checker_texture(texture* t0, texture *t1): even(t0), odd(t1){}
    virtual vec3 value(float u, float v, const vec3& p)const {
        float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if(sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }
    texture * odd;
    texture * even;
};


class noise_texture : public texture {
public:
    noise_texture(){}
    noise_texture(float s) :scale(s) {}
    virtual vec3 value(float u, float v, const vec3& p)const {
        return vec3(1,1,1)*0.5*(1 + sin(scale*p.z() + 10*noise.turb(p)));
    }
    float scale;
    perlin noise;
};

class image_texture : public texture {
public:
    image_texture() {}
    image_texture(unsigned char *pixels, int A, int B) : data(pixels), nx(A), ny(B) {}
    virtual vec3 value(float u, float v, const vec3& p) const;
    unsigned char *data;
    int nx, ny;
};

vec3 image_texture::value(float u, float v, const vec3& p) const {
    int i = static_cast<int>(u * nx);
    int j = static_cast<int>( ( 1 - v ) * ny - 0.001 );
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (i > nx-1) i = nx-1;
    if (j > ny-1) j = ny-1;
    float r = int(data[3 * i + 3 * nx * j]  ) / 255.0f;
    float g = int(data[3 * i + 3 * nx * j + 1 ]) / 255.0f;
    float b = int(data[3 * i + 3 * nx * j + 2 ]) / 255.0f;
    return vec3(r, g, b);
}
#endif //RAY_TRACING_TEXTURE_H
