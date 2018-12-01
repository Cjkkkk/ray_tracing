cmake_minimum_required(VERSION 3.12)
project(ray_tracing)

set(CMAKE_CXX_STANDARD 14)

add_executable(ray_tracing main.cpp vec3.cpp vec3.h ray.cpp ray.h hitable.cpp hitable.h sphere.cpp sphere.h hitablelist.cpp hitablelist.h camera.cpp camera.h)