---
layout: post
title: "需要齐次坐标的原因之二 - 所有的变换运算（平移、旋转、缩放）都可以用矩阵乘法来搞定"
date: 2014-04-23 13:55
comments: true
categories: OpenGL
---

看三个图就清楚了。分别是平移、旋转和缩放的矩阵计算。如果没有额外的第四个量w（也就是齐次坐标），那么这三种变换是无法都使用矩阵乘法来解决的。变换都统一成矩阵乘法之后，就给硬件设计和软件架构带来了巨大的好处。

<!-- more -->

下面的三组公式就分别对应使用矩阵来实现平移（Translation）、旋转（Rotation）、缩放（Scale）：

![Translation](/downloads/image/matrix-translation.png)

![Rotation](/downloads/image/matrix-rotation.png)

![Scale](/downloads/image/matrix-scale.png)
