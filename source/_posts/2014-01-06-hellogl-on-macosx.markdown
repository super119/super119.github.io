---
layout: post
title: "OpenGL Programming on MacOS X (Mavericks)"
date: 2014-01-06 21:30
comments: true
categories: OpenGL
---

最近开始看《OpenGL Programming Guide 8th Edition》一书。所以很自然的，就会想把书中的源代码在MacOS X Mavericks上试验一遍。在试验的过程中，自然也发现了一些书中没有说到的内容，所以记录下来。

事实上，这本书中的代码是在Windows上测试的，所以MacOS X上就会有一些不同。尤其体现在一些第三方的framework/library上。

本文是在试验该书中第一个源程序的时候，发现的一些东西。和我们第一个编写的HelloWorld程序一样，我称该程序为HelloGL。

<!-- more -->

首先给出我修改过的，能在Mavericks + Xcode 4.6.1上运行的程序的代码，相比书中的代码，有了一些改动。至于为什么要做这些改动，下面会详细描述。

{% include_code HelloGL主程序 c++ HelloGL/main.cpp %}

{% include_code LoadShaders c++ HelloGL/LoadShaders.cpp %}

{% include_code LoadShaders头文件 cpp HelloGL/LoadShaders.h %}

