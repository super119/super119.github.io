---
layout: post
title: "kernel: 调用init程序的地方"
date: 2015-12-21 13:57
comments: true
categories: linux-kernel
---

start_kernel函数的最后一行：rest_init，调用了init程序。

看代码就很清楚了。有个地方值得注意的是，优先被调用的是initramfs中的init程序（可以通过rdinit=xxx来设定）。

然后才会去检查 `init=` 的设置。

如果这两个都没有设置的话，kernel就会挨个去尝试/sbin/init, /bin/init, /bin/sh...

这些都没有那就panic了 
