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
可以表示点point 向量vector 颜色color 
```
float e[3]
```


## 3 ray
包括起始点origin和射线方向direction
```
vec3 origin;
vec3 direction;
```
p(t) = A + B * t

[![FnnwXn.md.png](https://s1.ax1x.com/2018/12/01/FnnwXn.md.png)](https://imgchr.com/i/FnnwXn)
图中的长方形规定了ray的出射范围

## 4 add sphere
### 4.1 组成
* 中心center C
* 半径radius R

### 4.2 计算光线是否碰到球体
设球上任何一点 P,以及射线ray = A + B * t,半径radius = r

则有等式dot(P - C, P - C) = r * r

展开得到 => dot(A + B* t - C, A + B* t - C) = r * r
          => dot(A - C, A - C) + 2* t * dot(B, A - C) + t * t* dot(B, B) - r*r = 0


## 5 surface normal and multiple objects

normal : unit vector 法向量

为 P - C后变为单位向量

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
对于多个物体的case, 每一个射出的光线计算与所有物体的碰撞关系, 选择最近的t作为碰撞点

## 6 anti aliasing 抗锯齿

在单位像素内进行取样平均

之前: 1/nx 2/nx 3/nx ...

改进: 1/nx 用 (1.1/nx + 1.2/nx + ...) / ns 来代替


## 7 material 材质 diffuse 漫反射
### 7.1 设计choice
将材质和几何分离

### 7.2 diffuse material(lambertian) 特性
* 不会发射光线
* 吸收入射光线(衰减)
* 随机方向反射

### 7.3 gamma矫正(gamma corrected)
原因: 韦伯定理 
人眼的线性感知与自然界刺激的线性变化是不一致的
比如在黑暗房间内点亮一根蜡烛可以感受到明显的亮度的变化
在点亮1000根蜡烛之后再点亮蜡烛则不会感受到明显的亮度变化

即在总量较小的时候的增量更容易被感知,在总量较大的时候相同的增量的感知不易察觉

albedo 反射率

## 8 Metal 镜面反射
### 8.1 特性
* 镜面反射
* 吸收能量

## 9 Dielectrics 电解质
透明物体比如 水 玻璃 钻石是电解质
### 9.1 特性
* 当光线穿过的时候分成反射和折射两种光线
* attenuation == 1 因为透明材料不吸收能量

### 9.2 模拟方法
* 随机在反射和折射中选择一种 这样就不用追踪两种光线
* 在发生全发射的时候一定选择反射

### 9.3 snell law折射定律
`nsin(theta) = n'sin(theta')`
一般来说空气折射率为1 玻璃为1.3-1.7 钻石为2.4

### 9.4 扩展
玻璃的折射率随着角度的变化而发生变化
Christophe Schlick的一个多项式近似

## 10 camera
相机不一定在原点
### 10.1 组成
* fov field of view 视野角度即原点为起点，在y方向长成的角度
* aspect 长宽比
* lookfrom 即从那里看
* lookat 即看向哪里
* 相机绕着 lookfrom-lookat 轴的旋转角度 upvector
最后得到ray扫描的范围

### 10.2 理解upvector

可以理解为即使一个人正对着对面的物体，也有倒立看和躺着看的情况，这里人的位置就是lookfrom
观察对象的位置就是lookat，可以看到即使两点确定，仍然有多种观看方式，所以需要锁定up vector


### 10.3 如何获得相机坐标系坐标轴

得到lookfrom与lookat连成的向量w
然后用叉乘得到与w和vup垂直的向量u
最后用叉乘得到与w和u垂直的向量v

## 11 defocus Blur (depth of field)
all photographers call it depth of field 景深

### 11.1 现实中产生景深的原因是
* 透镜只能聚焦一定距离的光线
* 超过这个距离会产生弥散圆, 弥散圆的半径在一定范围内是人眼无法察觉的

在产生人眼无法识别的弥散圆距离的范围是景深

### 11.2 影响因素
模型图[待补充]

因此景深和光圈和焦距有关系

### 11.3 模拟方法
模型图[待补充]
* 将lookfrom从一个点变化到一个圆平面,即光线不再是从一个点射出而是从一个圆平面射出的
* 同时无需模拟相机内部,只需要一个virtual film plane,将光线从virtual film plane射出即可

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

```
if (ray hits bounding object)
    return whether ray hits bounded objects
else
    return false
```

需要明确的是我们在划分object到subset并没有在划分空间

一个物体只能在一个bounding volume 但是bounding volume可以重叠

为了使得搜索是子线性(sub-linear)的,我们需要使得bounding volume层级关系,也可以说是树状态

            A

           / \

          B   C

         / \ / \

        D  E F  G

       如果射中了A则可能射中了B或者C,如果射中了B则可能会射中D或者E ...
