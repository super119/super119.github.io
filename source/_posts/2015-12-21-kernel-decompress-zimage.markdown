---
layout: post
title: "kernel: How to decompress the zImage?"
date: 2015-12-21 14:00
comments: true
categories: linux-kernel
---

zImage不是标准的gzip文件，需要自己strip掉开头的一些东西才可以gunzip。

基本上的思路就是找gzip的magic number "1f 8b 08"。

<!-- more -->

具体步骤：

``` bash
$ mkdir -p /tmp/kernel-uncompressed/; cd /tmp/kernel-uncompressed/
$ cp /boot/vmlinuz-`uname -r` .
$ od -t x1 -A d vmlinuz-2.6.18-128.el5.uvm6PAE | grep "1f 8b 08"
0008320 1b 00 1f 8b 08 00 d5 c2 9a 49 02 03 ec 3b 7d 7c
$ dd bs=1 skip=8322 if=vmlinuz-2.6.18-128.el5.uvm6PAE | zcat > vmlinux
```
