---
layout: post
title: "卓普大黑（C7、zp990）增大data分区"
date: 2015-04-05 18:18
comments: true
categories: Android
---

现在的Android APP对空间的使用是越来越大，APP本身一般都有几十M，再加上使用过程中也会消耗大量存储，比如微信就经常会使用到上百M空间。

卓普大黑的默认data分区大小是3G，随着安装的APP越来越多，data分区就经常会被耗尽。所以，增大data分区就顺理成章的成为一个需求。

<!-- more -->

卓普大黑使用的是MTK的方案，所以以下描述的方法对MTK 6589（6589T）的机器都适用。本文主要参考了：

[http://forum.xda-developers.com/showthread.php?t=2455850&nocache=1](http://forum.xda-developers.com/showthread.php?t=2455850&nocache=1)

方法很简单，就是修改线刷包中的 `EBR1` 和 `EBR2` 这两个文件，因为这两个文件定义了System, cache, data等诸多分区的大小。所以，在上面给出的帖子中就有下载对应的 `EBR1` 和 `EBR2` 这两个文件的。我的大黑是32G的版本，选择的是将data分区扩大到7GB。

由于上述的帖子中将 `EBR1` 和 `EBR2` 这两个文件是放在了4share上，这个网站是被墙的，所以这里将我使用的 `EBR1` 和 `EBR2` 文件放在这里：

[ebr-mtk-6589-partizione-7gb.zip](/downloads/file/ebr-mtk-6589-partizione-7gb.zip)

再次重申这两个文件我是在我的大黑上测试OK的，虽然上面给出的帖子中说MTK的 6577/ 6582/ 6589 /6592 /8382 都可以，但是我只测试过6589。

使用这两个文件替换掉线刷包里面原来的同名文件，然后刷机即可。

没有使用过线刷的，可以看我之前的一篇文章：

[卓普大黑（Zopo C7/zp990）刷原生ROM，root，升级触控屏，修改GPS](http://markzhang.cn/blog/2014/06/08/zopo-c7-flash/)

刷完之后就可以看到data分区变成了7GB：

![Zopo C7 7GB data partition](/downloads/image/zopo-c7-7gb-data-partition.jpg)
