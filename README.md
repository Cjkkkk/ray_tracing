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


## vec3
```
float e[3]
```

## ray
```
vec3 origin;
vec3 direction;
```

## add sphere
set center = C

set any point on sphere P

set ray = A + B * t

set radius = r

then sphere equation = dot(P - C, P - C) = r * r

expend it => dot(A + B*t - C, A + B*t - C) = r * r
          => dot(A - C, A - C) + 2*t*dot(B, A - C) + t*t*dot(B, B) - r*r = 0

