# ray_tracing
# chapter 1
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

## vec3
```
float e[3]
```

2.ppm

## ray
```
vec3 origin;
vec3 direction;
```

3.ppm

## add sphere
set center = C

set any point on sphere P

set ray = A + B * t

set radius = r

then sphere equation = dot(P - C, P - C) = r * r

expend it => dot(A + B*t - C, A + B*t - C) = r * r
          => dot(A - C, A - C) + 2*t*dot(B, A - C) + t*t*dot(B, B) - r*r = 0

4.ppm


## surface normal and multiple objects

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

## anti aliasing 抗锯齿

average samples in one pixel

before: 1/nx 2/nx 3/nx ...

after: 1/nx can be average by (1.1/nx + 1.2/nx + ...) / ns
