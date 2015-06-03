---
layout: post
title: "NVIDIA Linux驱动在Ubuntu 12.04上的安装"
date: 2015-06-03 21:35
comments: true
categories: linux
---

在Ubuntu上安装显卡驱动不是一件容易的事情，安装不是Ubuntu官方认证过的NVIDIA自家的驱动就更是困难，而我选择在一个老的Ubuntu版本上安装显卡驱动，事情就变的更加麻烦起来。

其实一开始我也不想安装NVIDIA自家的驱动，首先想尝试用 `nouveau`，但是悲剧的发现Ubuntu 12.04上的nouveau无法驱动我的GTX970，可能需要新版本的Ubuntu才可以。所以，被迫选择安装NVIDIA自家的驱动。

<!-- more -->

安装和使用N卡驱动中途遇到了很多的问题，这里来个总结。

## 第一次安装 ##

现在总结起来，可以按照如下步骤来：

- 禁用nouveau和其他相关的module：

``` bash
sudo vi /etc/modprobe.d/blacklist.conf

添加：

blacklist vga16fb
blacklist nouveau
blacklist rivafb
blacklist nvidiafb
blacklist rivatv
```

- 卸载nvidia相关的package，否则NVIDIA驱动安装的时候会报错：

``` bash
dpkg -l|grep nvidia
sudo apt-get remove <nvidia packages>
```

- 安装一些常用的package，比如：

``` bash
sudo apt-get install build-essential
sudo apt-get install ia32-libs
```

这是因为NVIDIA驱动安装的时候，会编译kernel module（nvidia.ko），所以需要一些常用的开发用的package。

- NVIDIA驱动安装的时候，X server不能运行，所以我是这么做的：

1. 重启系统，在grub界面选择进入 `Recovery` 模式
2. 进入Recovery之后，选择 `Enable Network`
3. 重新返回到Recovery界面，选择 `Drop to root console`
4. 运行NVIDIA驱动安装包，按照提示操作即可

之所以要做一次 `Enable Network` 是因为，这个选项会mount一些必要的文件系统，比如 `/tmp`，NVIDIA驱动安装的时候是需要 `/tmp` 的。

## Ubuntu更新kernel之后 ##

如果你使用了Ubuntu的 `Update Manager` 更新了package之后，一定要注意是否有升级kernel，如果升级了kernel，那么很可能你需要重新安装一次NVIDIA的驱动。

这是因为上面也提到了，`nvidia.ko` 需要重新编译。所以，如果你更新了kernel，发现X server起不来了，那么就可以尝试这么来：

1. 重启系统，在grub界面选择进入 `Recovery` 模式
2. 进入Recovery之后，选择 `Enable Network`
3. 重新返回到Recovery界面，选择 `Drop to root console`
4. 运行 `nvidia-uninstall` 卸载当前的NVIDIA驱动
5. 重新运行NVIDIA驱动安装包，再安装一遍驱动

之前我尝试过不运行 `nvidia-uninstall`，直接再次安装一遍驱动，发现这样做的话，安装包也会提醒你说发现当前已经安装了驱动，说是否需要卸载再重新安装。然后我就选择OK，但是发现这样操作下来之后有的时候还是不行，所以还是自己手动uninstall，然后再安装吧。

## 如何看出错日志 ##

主要就是要看两个东西：

- dmesg：这是kernel的log，看nvidia的内核驱动是否有什么错误，crash，如果有，就Google一下错误信息看是否有帮助

这里提一下，看到这个信息是正常的：

``` bash
[    1.222765] nvidia: module license 'NVIDIA' taints kernel.
[    1.225367] nvidia: module verification failed: signature and/or  required key missing - tainting kernel
```

- /var/log/Xorg.0.log：这里主要是看NVIDIA的X server driver（nvidia_drv.so）是否有错误，crash

## 万般无奈怎么办 ##

- 去 [http://www.geforce.com/drivers](http://www.geforce.com/drivers) 看看有没有BETA版本的驱动下载，有的话下载下来试试。再说一次，记得先uninstall当前的驱动再安装新的。

- 去NVIDIA的论坛报bug，我没这么干，因为本身我用的Ubuntu就是老版本了，估计NVIDIA也不会理我，如果去报告Ubuntu 14.04上的bug，可能NVIDIA会创建bug并派人fix。
