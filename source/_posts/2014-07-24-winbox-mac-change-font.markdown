---
layout: post
title: "Winbox for Mac OS X，修改界面字体"
date: 2014-07-24 18:19
comments: true
categories: MacOSX network
---

RouterOS（或简称ROS）是拉脱维亚的Mikrotik开发的一个强大到变态（对于个人用户而言）的路由系统。基于Linux，支持x86和MIPS（routerboard）。也就是说，你可以找一台PC来，装上routeros，就变身成了一台路由器。所以也有很多人称RouterOS为软路由。

Winbox是Mikrotik开发的RouterOS的管理工具（客户端）。当然，RouterOS也有网页版和命令行（终端，支持SSH，串口等）的管理界面，但是Winbox是相对来说最直观和好用的（当然用多了肯定到最后就是纯命令行了）。

有一个问题就是，Winbox只有windows版的。在Mac和Linux上，要使用wine来运行（或安装虚拟机）。

<!-- more -->

对于Mac OS X，网上有爱好者已经做好了一个包，下载下来安装即可，不用自己去折腾安装和配置 `wine`。地址在这里：

[http://wiki.mikrotik.com/wiki/MikroTik_WinBox_for_Mac_StandAlone](http://wiki.mikrotik.com/wiki/MikroTik_WinBox_for_Mac_StandAlone)

的确很好用，就是有一个问题，字体太难看。看起来用的是默认的宋体，所以英文显示效果不忍直视。

所以很自然想修改字体。说起修改这个字体，其实就是修改windows的默认字体。参考了这篇文章：

[http://linfan.info/blog/2012/03/01/wine-mac/](http://linfan.info/blog/2012/03/01/wine-mac/)

具体可以这么做：

- 找一个喜欢的字体（Mac OS X自带了很多字体，如果安装了Office for Mac，那微软的很多字体也都有了），拷贝到 `/Applications/Winbox.app/drive_c/windows/Fonts` 目录下。比如我用的是微软雅黑：

``` bash
cp /Library/Fonts/Microsoft/Microsoft\ Yahei.ttf /Applications/WinBox.app/drive_c/windows/Fonts
```

- 使用Wine运行注册表编辑器。这个打包好的Winbox for Mac，使用的是Wineskin（就是一个wine的wrapper，这样就把winbox.exe做成了Mac上的Winbox.app），所以这样：

``` bash
open /Applications/WinBox.app/Wineskin.app/
```

然后点击 `Advanced` 按钮，再选择 `Tools` 标签页，最后点击 `Registry Editor (regedit)` 按钮，出现windows的注册表编辑器，导入这个注册表：

``` ini
REGEDIT4

[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\FontSubstitutes]
"Microsoft Yahei"="Microsoft Yahei"
"Comic Sans MS"="Microsoft Yahei"
"Courier"="Microsoft Yahei"
"Courier New"="Microsoft Yahei"
"Fixedsys"="Microsoft Yahei"
"Helv"="Microsoft Yahei"
"Helvetica"="Microsoft Yahei"
"Lucida Console"="Microsoft Yahei"
"Lucida Sans"="Microsoft Yahei"
"Microsoft Sans Serif"="Microsoft Yahei"
"MingLiU"="Microsoft Yahei"
"PMingLiu"="Microsoft Yahei"
"MS Shell Dlg"="Microsoft Yahei"
"MS Shell Dlg 2"="Microsoft Yahei"
"MS Dialog"="Microsoft Yahei"
"MS Gothic"="Microsoft Yahei"
"MS PGothic"="Microsoft Yahei"
"MS UI Gothic"="Microsoft Yahei"
"MS Mincho"="Microsoft Yahei"
"MS PMincho"="Microsoft Yahei"
"SimHei"="Microsoft Yahei"
"SimSun"="Microsoft Yahei"
"NSimSun"="Microsoft Yahei"
"System"="Microsoft Yahei"
"Tahoma"="Microsoft Yahei"
"Times"="Microsoft Yahei"
"Times New Roman"="Microsoft Yahei"
"Tms Rmn"="Microsoft Yahei"
"Trebuchet MS"="Microsoft Yahei"
"Verdana"="Microsoft Yahei"
```

这样就OK了，再次打开Winbox，终于看到舒服的界面了：

![Mac Winbox changed font](/downloads/image/mac-winbox-change-font.png)
