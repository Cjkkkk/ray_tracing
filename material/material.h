//
// Created by DELL on 2018/12/1.
//

#ifndef RAY_TRACING_MATERIAL_H
#define RAY_TRACING_MATERIAL_H

struct hit_record;

#include <math.h>
#include <algorithm>
#include "../ray.h"
#include "../Geometry/hitable.h"
#include "texture.h"
#include "../Utils/s_random.h"
#include "../vec3.h"

// 让折射率随着角度的变化而变化
float schlick(float cosine, float ref_idx) {
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

// 返回是否会折射出去 因为从玻璃内部向外部折射的时候 出射的角度的正弦^2 = n^2 * 入射角的正弦^2 可能大于1 此时无法折射出去
// ni_over_nt is n / n'
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);//得到cos
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * ( 1 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }else
        return false;
}


vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}


vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(drand48(),drand48(),drand48()) - vec3(1,1,1);
    } while (p.squared_length() >= 1.0);
    return p;
}


class material  {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
    virtual vec3 emitted(float u, float v, const vec3& p) const{
        return vec3(0, 0, 0);
    }
};

// 发光物体
class diffuse_light : public material  {
public:
    diffuse_light(texture *a) : emit(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const { return false; }
    virtual vec3 emitted(float u, float v, const vec3& p) const { return emit->value(u, v, p); }
    texture *emit;
};

// brdf

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = std::max<float>(dot(N, V), 0.0);
    float NdotL = std::max<float>(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

float D_GGX_TR(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = std::max<float>(dot(N,H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float denom = M_PI * pow ( NdotH2 * ( a2 - 1.0f ) + 1.0f, 2);

    return a2 / denom;
}

float schlick_(float cosine, float ref) {
    return ref + ( 1 - ref ) * pow(( 1 - cosine), 5);
}

class brdf : public material  {
public:
    brdf(texture *a, float F0_, float alpha_) : albedo(a), F0(F0_), alpha(alpha_) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        //fr=kdflambert+ksfcook−torrance
        // 出射光线
//        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
//        scattered = ray(rec.p, reflected, r_in.time());
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target-rec.p, r_in.time());
        vec3 N = rec.normal; // 法向量
        vec3 V = -r_in.direction();
        vec3 L = scattered.direction();
        N.make_unit_vector();
        V.make_unit_vector();
        L.make_unit_vector();
        vec3 H = V + L;
        H.make_unit_vector();
        float cosTheta = std::max<float>(dot(N, L), 0.0); // 入射光线与法线的夹角
        float sch = schlick_(cosTheta, F0);
        vec3 lambert = ( 1 - sch) * albedo->value(rec.u, rec.v, rec.p) / M_PI;
        vec3 cook_torrance = sch
                * D_GGX_TR(N, H, alpha)
                * GeometrySmith(N, V, L, alpha*alpha / 2)
                * vec3(sch, sch, sch) / ( 4 * std::max<float>(dot(L, N), 0.0f) * std::max<float>(dot(V, N), 0.0f) + 0.001f);
        attenuation = (lambert + cook_torrance) * cosTheta * 5;
        //std::cout << " " << sch << " " << attenuation << "\n";
        return true;
    };
    float F0 , alpha;
    texture *albedo;
};

class lambertian : public material {
public:
    lambertian(texture* a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target-rec.p, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    texture *albedo;
};

class metal : public material {
public:
    metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
    vec3 albedo;
    float fuzz;
};

class dielectric : public material {
public:
    dielectric(float ri) : ref_idx(ri) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const  {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        // 判断光线是从高折射到低折射还是相反
        // 从玻璃射出时候二者同向
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx; // 入射材质与折射材质的反射率之比
            cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sqrt(1 - ref_idx*ref_idx*(1-cosine*cosine));
        }
        // 从空气射向玻璃
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0f / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        // 判断是否发生全反射
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
            reflect_prob = schlick(cosine, ref_idx);
        else
            reflect_prob = 1.0;
        // 如果为全反射则为反射光线 否则随即反射或者折射
        if (drand48() < reflect_prob)
            scattered = ray(rec.p, reflected, r_in.time());
        else
            scattered = ray(rec.p, refracted, r_in.time());
        return true;
    }

    float ref_idx;
};

#endif //RAY_TRACING_MATERIAL_H
