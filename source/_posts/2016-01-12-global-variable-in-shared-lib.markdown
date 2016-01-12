---
layout: post
title: "链接装载和库读书笔记 - 在动态库中尽量少使用全局变量，要使用请加上static"
date: 2016-01-12 09:39
comments: true
categories: linux
---

动态共享库编译的时候，对于全局变量，一律视为模块间变量访问。变量会通过数据段的GOT表来访问。而且，当可执行文件中（由于可执行文件不是PIC的）有同名全局变量时，共享库中定义的变量会被ignore，进程中只会存在可执行程序中的super_a。

<!-- more -->

所以，编写动态库的时候，尽量少用全局变量，如果要用，请加上static，这样就不会有问题。

参考链接装载和库的P198 -- 共享模块的全局变量问题。

BTW：如果是静态链接，链接会失败报错 -- multiple definition of super_a。 

以下是测试代码：

``` cpp
super@super-desktop:~$ cat lib.h
#ifndef _LIB_H
#define _LIB_H

void lib_func();

#endif
super@super-desktop:~$ cat lib.c
#include <stdio.h>

int super_a = 0;

void lib_func()
{
    printf("A is: %d\n", super_a);
    return;
}
super@super-desktop:~$ cat main.c
#include <stdio.h>
#include "lib.h"

int super_a = 0;

int main()
{
    super_a = 100;
    lib_func();
    printf("A in main is: %d\n", super_a);
    return 0;
}
```

输出是：

``` bash
super@super-desktop:~$ gcc -shared -fPIC -g -o liblib.so lib.c
super@super-desktop:~$ gcc -o main main.c -L. -llib
super@super-desktop:~$ ./main 
A is: 100
A in main is: 100
```
