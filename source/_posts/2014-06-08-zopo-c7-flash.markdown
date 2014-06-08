---
layout: post
title: "卓普大黑（Zopo C7/zp990）刷原生ROM，root，升级触控屏，修改GPS"
date: 2014-06-08 15:00
comments: true
categories: Android
---

卓普（Zopo）大黑（C7）手机在国内的行货默认安装的是阿里的云OS，而向国外销售的版本称为zp990，装的是原生的安卓（当然有一些针对这款手机的改动，比如FOTA部分，双卡设置部分等）。

如果你用云OS用腻了的话，肯定会想刷机，而要刷机，刷zp990的原生安卓版本成为一个好的选择。我的使用感受是，这个版本相比其他的民间ROM（比如民间做的MIUI的ROM）要稳定一些，所以你不想经常折腾的话，刷这个版本会安逸很多。

本文中所有内容都是来自：

- 移动叔叔论坛（bbs.ydss.cn），卓普大黑版块
- [http://forum.xda-developers.com/showthread.php?t=2403825](http://forum.xda-developers.com/showthread.php?t=2403825)

<!-- more -->

## 准备工作 ##

刷机需要以下一些东西：

- ROM，这里我使用的是移动叔叔论坛上arboby提供的线刷包：

[http://bbs.ydss.cn/thread-355478-1-1.html](http://bbs.ydss.cn/thread-355478-1-1.html)

这里说明两点：

1. 之所以采用线刷包，是因为相比在Recovery下刷机，你不需要首先去root手机，然后安装第三方的recovery，最后再刷入zip格式的ROM包。线刷包只需要你安装手机的线刷驱动（对于卓普这款MTK的手机来说，就是安装MTK Preloader驱动），然后运行MTK的Flash工具即可，非常的简单快速。

2. arboby提供的这个线刷包，相比原生的我上面提到的，卓普在海外销售的zp990的ROM，只修改了一个image，就是recovery image。这样我们线刷一次，就得到了ROM和第三方的recovery，非常的方便。其实我下载过原生的zp990的ROM，跟arbody的ROM做过diff，只有一个recovery image是不同的。

- MTK的线刷软件：SP Flash tool。

- MTK线刷的驱动，也就是上面提到的MTK Preloader的驱动。

所有上述的东西在我的百度盘上都可以下载到：[http://pan.baidu.com/s/1hq5klU0](http://pan.baidu.com/s/1hq5klU0)。

## 开始刷机 ##

- 首先是安装线刷驱动，这里移动叔叔论坛上有了很不错的教程：

[http://bbs.ydss.cn/thread-343673-1-1.html](http://bbs.ydss.cn/thread-343673-1-1.html)

我是在Windows 7 64bit上安装的。论坛上很多人说推荐使用XP的系统，因为win7，尤其是64bit的win7很难安装。其实我的感觉不是这样，我感觉很多人安装不上这个驱动是因为他们使用的是Ghost win7。只要是正常安装的win7，无论是否64bit，安装这个驱动都是没有问题的。

其次就是注意要安装这个驱动，是手机关机、拔下电池，然后连接USB到电脑上。此时电脑上的设备管理器中就会出现一个未知设备，然后按照移动叔叔上那个教程一步一步做就可以了。安装好了之后，这个设备就叫MTK Preloader ...，而且这个设备会不停的消失、出现，这是正常现象。只要一旦开始刷机，这个设备就不会再不停的消失出现了。

我之前有在虚拟机的win7上尝试安装过这个驱动，感觉要困难一些（也不是完全不可能）。因为对于虚拟机来说，所有设备是先到Host system上，然后你需要去手动选择将该设备切换到虚拟机中来。考虑到这个坑爹的MTK的设备会不停的出现消失，这就增加了安装的难度。所以简单来说，不想找麻烦就别在虚拟机的windows上安装了吧。

- 然后就是运行MTK的Flash tool刷机了。过程超级简单，就是把刷机包中的bootloader, kernel, system.img, recovery.img等等全部刷到手机的EMMC中的不同的分区上即可（当然bootloader一般是刷到SPI ROM上，我没研究过MTK的这个逻辑）。移动叔叔上也有教程：

[http://bbs.ydss.cn/thread-344988-1-1.html](http://bbs.ydss.cn/thread-344988-1-1.html)

最后刷完就会看到一个对话框，里面是一个绿色的圆。使用一张移动叔叔论坛上的图装饰一下：

![MTK SP Flash Tool](/downloads/image/mtk-flash-tool.jpg)

刷完之后开机就可以看到原生的安卓了。这里有一个小插曲，在我上面给出的arboby线刷包的link上也提到了，就是刷完之后，你会发现你的EMMC只有3G。我的卓普大黑是32G的版本，我没有额外再安装SD卡了，所以应该是一个3G的data分区（用来安装APK），剩下来会有一个25G多的分区作为/sdcard分区。

对于这个问题，arboby在帖子中说，只需要进入到系统设置中，做一次 `恢复出厂设置` 就可以。这个方法在我的手机上不行，在我的大黑上，做了一次 `恢复出厂设置` 后，系统重启后会卡死，无法进入Android。我试验过两次都是这样。所以我后来使用了另外一种方法，就是进入Recovery去做一次 `双清` ，也就是format data和cache两个分区。

说到这里，又要提到一个该手机和其他手机不一样的地方了。该手机不是按住音量的上或者下，然后按power键开机就进入了 `recovery` 模式。该手机进入 `recovery` 的方法是：

** 手机在关机状态下，按住音量+，然后将USB线连接到手机上并插入电脑 **

所以并不是按power开机，而是按住 音量+ 然后连接USB线，就会自动进入recovery了。

在xda-developers上，采用的方法是在PC端运行一个叫做 `MTKDroidTools` 的软件从而进入recovery，我没有试验过因为我觉得这种方法太麻烦。

## 后续 ##

刷机完成了，貌似结束了，其实还有一些东西是可以做的（当然不做也可以，看个人需要）：

- Root手机。其实root手机会带来安全隐患，但是为了升级触控屏的firmware（下面会提到），必须要先root。root就使用arboby提供的一键root的那个程序即可。前提条件是，PC上要安装好Android adb驱动并且手机上要打开USB调试（老生常谈了）。ADB的驱动我是直接在PC上安装了91助手，然后连接手机，91助手自动帮我安装的ADB驱动，你也可以去网上搜索找一下这个驱动，就是MTK的ADB驱动。

- 升级触控屏（touch screen）的firmware。我的手机上，这个firmware的版本是0x14，根据arboby的描述，升级到0x17版本就可以完美支持 `小皮套`。这个皮套我也有一个，就是可以合上休眠，打开唤醒，同时还有一个透明窗口可以在不开皮套的情况下接电话。虽然我平常不使用这个，但是我认为升级一下还是有必要的。

- 修改GPS的一个设置。这个来自xda-developers，据称不修改这个GPS的定位将会非常的慢。具体的做法倒是特别简单（但是也需要先root手机），就是编辑文件：`/system/build.prop`，在最后面加入一行：`Dalvik.vm.execution-mode=int:fast` ，然后重启手机即可。

- 最后是调手机通话音量的，这款手机几乎所有人都知道有个问题就是通话音量太小，不是你听对方的声音小，是对方听你的声音小。据说原因是手机麦克风外面的防尘网做的太密？也据说论坛上有人暴力将这个防尘网的缝隙扩大了一些，立马声音变的很洪亮？但是如果不想做硬件上的HACK，那有一个设置是可以做的，就是增加手机麦克风的灵敏度（航模上我们叫感度）：

[http://bbs.ydss.cn/thread-367161-1-1.html](http://bbs.ydss.cn/thread-367161-1-1.html)

## 结束 ##

整个Android手机就意味着折腾的开始，不喜欢的折腾的，还是买iPhone吧。
