---
layout: post
title: "链接装载和库读书笔记 - 未初始化的全局变量是弱符号"
date: 2016-01-12 09:43
comments: true
categories: linux
---

代码见文章末尾。

a是未初始化的全局变量，编译成.o的时候不会被放入.bss段，是一个弱符号。当有2个或多个弱符号的时候，linkder会挑选占用空间最大的弱符号。所以，这里最后链接完成后，a占用8个字节。但是在a.c中，a的类型仍然是int，在b.c中，a的类型是double。只是占用空间是8个字节。

<!-- more -->

至于输出结果，由于b.c给a赋值了300，而且b.c中a是double类型，所以内存中a是：

``` bash
(gdb) x/1xg 0x601030
0x601030 <a>:    0x4072c00000000000
``` 

这就是为什么最后在a.c中打印a，出来的值是0。

更多解释看链接装载和库的P111 -- common块。 

BTW：如果两个a都是初始化了的全局变量，则链接会失败，因为两个强符号无法共存。如果一个初始化一个不初始化，则一个强符号一个弱符号，则以强符号占用的空间为准。 

测试代码如下：

``` cpp
super@super-desktop:~$ cat a.c 
#include <stdio.h>
#include "b.h"

int a;

int main()
{
    a = 100;
    b_process();
    printf("A in main is: %d\n", a);
    return 0;
}
super@super-desktop:~$ cat b.c 
#include <stdio.h>

double a;

void b_process()
{
    a = 300;
    printf("A in b is: %f\n", a);
}
super@super-desktop:~$ cat b.h
#ifndef _B_H
#define _B_H

void b_process();

#endif
```

输出是：

A in b is: 300.000000

A in main is: 0
