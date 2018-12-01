# ray_tracing
## chapter 1
```
PPM format
P3
[column] [row]
[max value]
[R][G][B]
[R][G][B]
...
...
[R][G][B]
```

tool:
[PPM预览器](http://paulcuth.me.uk/netpbm-viewer/)

1.ppm

## 2 vec3
```
float e[3]
```

2.ppm

## 3 ray
```
vec3 origin;
vec3 direction;
```

camera(eye) is in vec3(0.0, 0.0, 0.0)

[![FnnwXn.md.png](https://s1.ax1x.com/2018/12/01/FnnwXn.md.png)](https://imgchr.com/i/FnnwXn)

3.ppm

## 4 add sphere
set center = C

set any point on sphere P

set ray = A + B * t

set radius = r

then sphere equation = dot(P - C, P - C) = r * r

expend it => dot(A + B*t - C, A + B*t - C) = r * r
          => dot(A - C, A - C) + 2*t*dot(B, A - C) + t*t*dot(B, B) - r*r = 0

4.ppm


## 5 surface normal and multiple objects

normal : unit vector

```
hitable{
    virtual bool hit();
}
```

```
sphere:public hitable
```

```
hitable_list: public hitable
```

## 6 anti aliasing 抗锯齿

average samples in one pixel

before: 1/nx 2/nx 3/nx ...

after: 1/nx can be average by (1.1/nx + 1.2/nx + ...) / ns


## 7 material 材质 漫反射
mix material and geometry or separate ?

start with separate

diffuse material

absorb light

approximate Lambertian

gamma corrected

choice
* scatter always and attenuate by its reflectance R
* scatter with no attenuation but absorb fraction 1 - R of the rays
* mixture of those strategies


albedo 反射率

## 8 Metal 镜面反射
## 9 Dielectrics
透明物体比如 水 玻璃 钻石是电解质
当光线穿过的时候分成反射和折射两种光线

方法 随机在反射和折射中选择一种 这样就不用追踪两种光线

snell law折射定律
nsin(theta) = n'sin(theta')
一般来说空气折射率为1 玻璃为1.3-1.7 钻石为2.4

问题 完全内反射

ps attenuation == 1 因为透明材料不吸收能量