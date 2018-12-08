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

扩展
玻璃的折射率随着角度的变化而发生变化
Christophe Schlick的一个多项式近似

## 10 camera
field of view 视野角度
即原点为起点，在y方向长成的角度
然后定义长宽比
最后得到ray扫描的范围

扩展相机
=> 不一定在原点

需要有
lookfrom 即从那里看
lookat 即看向哪里
以及相机绕着 lookfrom-lookat 轴的旋转角度 upvector
可以理解为即使一个人正对着对面的物体，也有倒立看和躺着看的情况，这里人的位置就是lookfrom
观察对象的位置就是lookat，可以看到即使两点确定，仍然有多种观看方式，所以需要锁定up vector

得到lookfrom与lookat练成的向量(a,b,c)
首先得到与它垂直的向量(-b,-a,0)

## 11 defocus Blur
all photographers call it depth of field
景深

现实中产生景深的原因是相机需要大的hole来收集光线

aperture

无需模拟film - lens - focus plane的结构
直接可以把film lens合成lens plane

# next week
hardest part BVH/ Perlin textures

all effects can be brute-forced

## 12 Motion blur
形成原因: 在真实的相机中, 快门打开之后会维持一段时间的打开状态
这个时候相机或者物体会保持运动

我们可以让相机在这个运动过程中取一个平均值作为我们的视觉效果

获取到到的每一个ray是在一个时间段内random生成的
比如[1,10]这个时间段
则所有的ray可以是[1,10]中的任意时刻射出

## 13 Bounding Volume Hierarchies
ray-object intersection检测是ray tracer中的主要时间瓶颈

在相同的模型上有许多的重复搜索,应该可以使用二分搜索变为对数时间

将模型分类然后将每一此ray-object intersection变为sublinear search

如何对模型分类?
* 空间上分类(divide the space)
* 物体分类(divide the object) easy to code up

核心思想是用一个bounding volume来包住一堆的object来找到一个能完全包住所有object的bounding volume. 比如你已经有一个包住10个object的bounding
volume. 那么所有没有射中bounding volume的光线一定不会射中他包住的10个物体. 如果射中了bounding volume,那么才有可能射中10个中的一个.
ray object intersection的一般形式

if (ray hits bounding object)
    return whether ray hits bounded objects
else
    return false

需要明确的是我们在划分object到subset并没有在划分空间

一个物体只能在一个bounding volume 但是bounding volume可以重叠

为了使得搜索是子线性(sub-linear)的,我们需要使得bounding volume层级关系,也可以说是树状态
            A
           / \
          B   C
         / \ / \
        D  E F  G
       如果射中了A则可能射中了B或者C,如果射中了B则可能会射中D或者E ...
