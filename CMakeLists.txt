cmake_minimum_required(VERSION 3.12)
project(ray_tracing)

set(CMAKE_CXX_STANDARD 14)

add_executable(ray_tracing main.cpp vec3.h ray.h hitable.h sphere.h hitablelist.h camera.h material.h)