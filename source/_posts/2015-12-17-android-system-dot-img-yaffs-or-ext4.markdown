---
layout: post
title: "Android system.img: yaffs or ext4?"
date: 2015-12-17 11:18
comments: true
categories: Android
---

随着现在手机的内部存储从NAND转变到eMMC，现在的system.img已经越来越多的使用ext4作为文件系统，而不是以前的yaffs2。

使用file命令查看system.img，如果输出是：VMS Alpha Exectutable，那么这是yaffs

<!-- more -->

如果输出是data，那么很可能是ext4的。只不过这个image是一个spare image，含有一些spare数据，所以需要先用simg2img处理一下，然后就可以直接mount了。

至于为什么采用eMMC来代替NAND，摘录百度百科的一段话：

> eMMC目前是最当红的移动设备本地存储解决方案，目的在于简化手机存储器的设计，由于NAND Flash芯片的不同厂牌包括三星、KingMax、东芝(Toshiba)或海力士(Hynix)、美光(Micron)等，入时，都需要根据每家公司的产品和技术特性来重新设计，过去并没有哪个技术能够通用所有厂牌的NAND Flash芯片。

> 而每次NAND Flash制程技术改朝换代，包括70纳米演进至50纳米，再演进至40纳米或30纳米制程技术，手机客户也都要重新设计，但半导体产品每1年制程技术都会推陈出新，存储器问题也拖累手机新机种推出的速度，因此像eMMC这种把所有存储器和管理NAND Flash的控制芯片都包在1颗MCP上的概念，逐渐风行起来。

> eMMC的设计概念，就是为了简化手机内存储器的使用，将NAND Flash芯片和控制芯片设计成1颗MCP芯片，手机客户只需要采购eMMC芯片，放进新手机中，不需处理其它繁复的NAND Flash兼容性和管理问题，最大优点是缩短新产品的上市周期和研发成本，加速产品的推陈出新速度。 
