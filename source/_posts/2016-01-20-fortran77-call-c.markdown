---
layout: post
title: "Fortran77如何调用C代码"
date: 2016-01-20 15:37
comments: true
categories: HPC
---

比如C的代码（文件名：test.c）如下（注意C源代码里不能有main函数，编译时也一定要加-c开关）：

<!-- more -->

``` cpp
#include <stdio.h>

#define sub1 sub1_

void sub1(int *num)
{
    printf("%d\n", *num);
}
```

Fortran的代码（文件名：testf.f）如下（Fortran调用C的sub1函数）：

``` bash
      program main
      implicit none

      integer a
      a = 10

      call sub1(a)

      end
```

这样就OK 了！编译的时候，这样操作：

``` bash
g77 -c testf.f -o testf.o
gcc -c test.c -o test.o
g77 test.o testf.o -o test
```

然后运行test程序就可以了。

相关说明：

1. C代码被编译后会在函数名前面或后面加上下划线，各编译器实现不同，这段代码是在gcc和intel的编译器上测试的，所以，在C的代码里要做一个define的动作

2. Fortran默认的函数参数传递方式是传址操作，所以，被调用的C函数的参数必须都是指针

3. 在Fortran90里，Fortran和C的函数互调基本上没有上述的限制
