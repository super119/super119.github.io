---
layout: post
title: "天语（k-touch） w619: Dump boot.img和system.img，然后做成recovery可用的刷机zip包"
date: 2015-12-17 11:08
comments: true
categories: Android
---

有的时候会有这样的问题：发现一个民间的刷机包很不错，但是就是缺少一些你想要的东西。这个时候，其实就可以尝试基于这个包自己做一些修改、添加，最终生成自己满意的系统包。

也有的时候，要给老人一部安卓机器来用，但是老人不擅长智能手机操作，经常会把一些APP误删除。这个时候其实可以自己制作刷机包，将必须的一些APP放到`system`下面，这样就不怕老人误删除APP了。

以下是我自己在天语w619的尝试：

<!-- more -->

1. root手机

2. 配置/etc/udev/rules.d/51-Android.conf，将手机的USB的vendor id/product id加进去。用lsusb可以看到这两个ID。

3. adb shell

4. su; cd /sdcard

5. tar czvf system.tar.gz /system

6. dd if=/dev/mtd/mtd0 of=/sdcard/boot.img  # 应该可以和system分区同样处理，用tar将/boot打包即可。dump整个分区有很多废数据产生

7. 退出adb shell

8. 在PC上：adb pull /sdcard/system.tar.gz

9. tar zxvf system.tar.gz

10. mkfs.yaffs2.x86 ./system system.img  # mkfs.yaffs2.x86在googlecode上下载

11. 有了boot.img和system.img，就可以使用Android Kitchen来制作一个刷机包了。 

 
cat /proc/mtd可以看到所有的mtd分区和对应的名字，这样就可以知道boot分区是哪个设备文件了。 

刷机时发现：

1. Android kitchen做的刷机包，META-INF/com/google/android目录下的update-binary是不正确的，可以找一个别人制作的w619刷机包中的替代。不正确的update-binary的现象就是刷机出错：status 0（使用的是CWM recovery）

2. META-INF/com/google/android目录下的updater-script中也有几个问题：

   A. 文件行尾不能有\r\n，只能有\r，也就是说必须是unix格式的文本。刷机现象：status 6

   B. format("MTD", "system"); -> format("yaffs2", "MTD", "system"); 刷机现象：CWM recovery会提示format有三个参数

   C. mount("MTD", "system", "/system"); -> mount("yaffs2", "MTD", "system", "/system"); # 和format的现象一样
