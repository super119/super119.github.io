---
layout: post
title: "mmap: invalid argument & MAP_PRIVATE"
date: 2015-12-17 10:53
comments: true
categories: linux
---

mmap，使用PROT_READ | PROT_WRITE, MAP_SHARED，总是返回EINVAL。

最后发现是因为被mmap的文件是一个network filesystem上的文件。

<!-- more -->

Linux实现MAP_SHARED是通过share文件的page cache做到的。对于一个网络上的文件来说，Linux无法做到文件的某个部分被修改了，同时打开该文件的其他机器上的程序能立马看到。

所以自然就返回EINVAL了。

悲催的是mmap的manual上并没有提到这种情况，所以查了半天。

此外，MAP_PRIVATE其实就是当你修改了文件之后，会copy一个新的page出来，所以自然就无法跟其他进程共享你的修改了。

同时还需要注意的是，MAP_PRIVATE的时候，对文件进行的修改将不会被写到文件中去！ 
