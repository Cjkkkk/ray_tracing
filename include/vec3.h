#ifndef RAY_TRACING_VEC3_H
#define RAY_TRACING_VEC3_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif

class vec3 {
public:
    CUDA_CALLABLE_MEMBER vec3(){}
    CUDA_CALLABLE_MEMBER vec3(const vec3& v) {
        e[0] = v[0]; e[1] = v[1]; e[2] = v[2];
    }
    CUDA_CALLABLE_MEMBER vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2;}
    CUDA_CALLABLE_MEMBER inline float x() const { return e[0];}
    CUDA_CALLABLE_MEMBER inline float y() const { return e[1];}
    CUDA_CALLABLE_MEMBER inline float z() const { return e[2];}
    CUDA_CALLABLE_MEMBER inline float r() const { return e[0];}
    CUDA_CALLABLE_MEMBER inline float g() const { return e[1];}
    CUDA_CALLABLE_MEMBER inline float b() const { return e[2];}

    CUDA_CALLABLE_MEMBER inline const vec3& operator+() const  { return *this; }
    CUDA_CALLABLE_MEMBER inline vec3 operator-() const {return vec3(-e[0], -e[1], -e[2]);}
    CUDA_CALLABLE_MEMBER inline float operator[] (int i) const {return e[i];}
    CUDA_CALLABLE_MEMBER inline float& operator[](int i) { return e[i];};

    CUDA_CALLABLE_MEMBER inline vec3& operator+=(const vec3 &v);
    CUDA_CALLABLE_MEMBER inline vec3& operator-=(const vec3 &v);
    CUDA_CALLABLE_MEMBER inline vec3& operator*=(const vec3 &v);
    CUDA_CALLABLE_MEMBER inline vec3& operator/=(const vec3 &v);
    CUDA_CALLABLE_MEMBER inline vec3& operator*=(const float t);
    CUDA_CALLABLE_MEMBER inline vec3& operator/=(const float t);

    CUDA_CALLABLE_MEMBER inline float length() const {
        return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
    };
    CUDA_CALLABLE_MEMBER inline float squared_length() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    };
    CUDA_CALLABLE_MEMBER inline void make_unit_vector();
    float e[3];
};


inline std::istream& operator>>(std::istream & is, vec3 &t){
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

inline std::ostream& operator<<(std::ostream & os, const vec3 &t){
    os << t.e[0] << " " << t.e[1] << " "<< t.e[2];
    return os;
}

CUDA_CALLABLE_MEMBER
inline void vec3::make_unit_vector(){
    float k = 1.0f / length();
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
}

CUDA_CALLABLE_MEMBER
inline vec3 operator +(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

CUDA_CALLABLE_MEMBER
inline vec3 operator -(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

CUDA_CALLABLE_MEMBER
inline vec3 operator *(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

CUDA_CALLABLE_MEMBER
inline vec3 operator /(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

CUDA_CALLABLE_MEMBER
inline vec3 operator *(float t, const vec3& v){
    return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

CUDA_CALLABLE_MEMBER
inline vec3 operator *(const vec3& v, float t){
    return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

CUDA_CALLABLE_MEMBER
inline vec3 operator /(const vec3& v, float t){
    return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

CUDA_CALLABLE_MEMBER
inline float dot(const vec3& v1, const vec3& v2){
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

CUDA_CALLABLE_MEMBER
inline vec3 cross(const vec3& v1, const vec3& v2){
    return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
            (- v1.e[0] * v2.e[2] + v1.e[2] * v2.e[0]),
            (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

CUDA_CALLABLE_MEMBER
inline vec3& vec3::operator+=(const vec3 &v){
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

CUDA_CALLABLE_MEMBER
inline vec3& vec3::operator-=(const vec3 &v){
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

CUDA_CALLABLE_MEMBER
inline vec3& vec3::operator*=(const vec3 &v){
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
}

CUDA_CALLABLE_MEMBER
inline vec3& vec3::operator/=(const vec3 &v){
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
}

CUDA_CALLABLE_MEMBER
inline vec3& vec3::operator*=(const float t){
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

CUDA_CALLABLE_MEMBER
inline vec3& vec3::operator/=(const float t){
    float k = 1.0f / t;
    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
}

CUDA_CALLABLE_MEMBER
inline vec3 unit_vector(vec3 v){
    return v / v.length();
}
#endif //RAY_TRACING_VEC3_H
