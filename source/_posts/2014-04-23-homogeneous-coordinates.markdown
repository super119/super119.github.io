---
layout: post
title: "需要齐次坐标的原因"
date: 2014-04-23 13:40
comments: true
categories: OpenGL repost
---

原文链接：[http://www.songho.ca/math/homogeneous/homogeneous.html](http://www.songho.ca/math/homogeneous/homogeneous.html)

翻译：罗朝辉 [http://www.cnblogs.com/kesalin/](http://www.cnblogs.com/kesalin/)

<!-- more -->

在欧几里得几何空间里，两条平行线永远都不会相交。但是在投影空间中，两条铁轨在地平线处却是会相交的，因为在无限远处它们看起来相交于一点。

在欧几里得（或称笛卡尔）空间里描述2D/3D 几何物体是很理想的，但在投影空间里面却并不见得。我们用 (x, y) 表示笛卡尔空间中的一个 2D 点，而处于无限远处的点 (∞,∞) 在笛卡尔空间里是没有意义的。投影空间里的两条平行线会在无限远处相交于一点，但笛卡尔空间里面无法搞定这个问题（因为无限远处的点在笛卡尔空间里是没有意义的）。

由 August Ferdinand Möbius 提出的齐次坐标（Homogeneous coordinates）让我们能够在投影空间里进行图像和几何处理，齐次坐标用N + 1个分量来描述 N 维坐标。比如，2D 齐次坐标是在笛卡尔坐标(X, Y)的基础上增加一个新分量 w，变成(x, y, w)，其中笛卡尔坐标系中的大X，Y 与齐次坐标中的小x，y有如下对应关系：

``` bash
X = x / w
Y = y / w 
```

笛卡尔坐标中的点 (1, 2) 在齐次坐标中就是 (1, 2, 1) 。如果这点移动到无限远(∞,∞)处，在齐次坐标中就是 (1, 2, 0) ，这样我们就避免了用没意义的"∞" 来描述无限远处的点。

** 证明: 两平行线可以相交 **

笛卡尔坐标系中，对于如下两个直线方程：

![直线方程](/downloads/image/linear-formula.png)
 
如果 C ≠ D，以上方程组无解；如果 C = D，那这两条线就是同一条线了。

下面我们用 x/w, y/w 代替 x, y 放到投影空间里来求解：

![齐次坐标直线方程](/downloads/image/linear-formula-homogeneous.png)
 
现在我们就可以在 C ≠ D 的情况得到一组解 (x, y, 0)，代入得 (C - D)w = 0，因为 C ≠ D，所以 w = 0。因而，两条平行线相交于投影空间中无限远处的一点 (x, y, 0)。

齐次坐标在计算机图形学中是有用的，将 3D 场景投影到 2D 平面的过程中就用到它了。
