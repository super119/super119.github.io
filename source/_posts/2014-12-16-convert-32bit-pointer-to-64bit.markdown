---
layout: post
title: "32/64的问题：32bit指针转换成64bit - Sign Extension"
date: 2014-12-16 15:09
comments: true
categories: linux-kernel
---

32/64有很多问题，指针转换是其中一个，比如这个例子：

<!-- more -->

``` cpp
void *p = 0xc8f68000;
unsigned long long v = (unsigned long long)p;
```

这个时候得到的 `v` 是： `0xffffffffc8f68000` 而不是 `0x00000000c8f68000`

以上例子来自：

[http://stackoverflow.com/questions/22239752/convert-32-bit-pointer-to-64-bit-integer](http://stackoverflow.com/questions/22239752/convert-32-bit-pointer-to-64-bit-integer)

最近就碰到了好几次类似的问题。原因非常简单，就是编译器看到我们要把一个指针转换成 `unsigned long long`，而 `p` 的最高位是1，编译器认为这是负数，所以编译器就启动了 `Sign Extension`，将高32bit全部设置成1，这样最终你看到的高32bit就是全F了。

所以当做32bit -> 64bit指针转换的时候，要使用 `intptr_t` 和 `uintptr_t`。这样编译器就知道这是指针的转换，高32bit就会是全0了。同样，这也就说明了，为什么 `stdint.h` 里面要定义这么些个typedef了:

``` cpp
void *p = 0xc8f68000;
uint64_t v = (uintptr_t)p;
```

事实上，无论何时都尽量要避免指针和Integer之间的转换，因为Integer是有正负的。
