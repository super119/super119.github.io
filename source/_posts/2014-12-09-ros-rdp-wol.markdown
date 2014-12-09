---
layout: post
title: "RouterOS/远程启动/控制家里的PC"
date: 2014-12-09 17:35
comments: true
categories: network
---

## 远程启动 ##

远程启动其实就是 `WOL(Wake on Lan)`，现在的PC机基本上都支持。需要两个东西：

- 在BIOS里面打开 `WOL`
- 使用一个程序，发送WOL的一个包（所谓magic packet）给PC机

先说第一个，在我的PC机上，这个设置是 Advanced Options -> APM -> Power on PCIE，设成 `Enabled` 即可。

<!-- more -->

再说第二个，很酷的是ROS就支持这个，这样你就不需要另外一台PC机常年开着了，ROS路由一直开着即可。在ROS中，打开 `New Terminal`，输入：

``` bash
/tool wol interface=<连着你要唤醒的PC机的那个网口> mac=<要唤醒PC机的网卡的MAC地址>
```

![ROS Wake on Lan](/downloads/image/ros-wol.png)

如果你的路由器不是ROS，或是没有WOL功能的话，那就需要另外一台PC来执行WOL的程序了。在Linux下，可以使用：

``` bash
sudo apt-get install etherwake
wakeonlan <mac address>
```

Windows下也一样，下载一个Wake on Lan的程序使用就行了。

## 远程控制 ##

首先就是打开Windows的远程桌面功能。在 `高级系统设置 -> 远程` 标签页里面：

![Windows RDP](/downloads/image/windows-rdp.png)

完了就是在ROS里面打开端口映射，这样你才可以从Internet上远程RDP到家里的PC机，需要映射的是著名的3389端口：

``` bash
chain=dstnat action=dst-nat to-addresses=<xxx.xxx.xxx.xxx> to-ports=3389 protocol=tcp dst-port=3389 
chain=dstnat action=dst-nat to-addresses=<xxx.xxx.xxx.xxx> to-ports=3389 protocol=udp dst-port=3389
```

这样就可以远程登录了。Ubuntu下面有自带的 `Remmina Remote Desktop` 可以使用，Mac下我用的是 `Jump Desktop`，个人感觉很好用。

最后一个小问题，有关远程登录机器开始菜单中没有 `关机` 这个功能，只需要打开 `Windows CMD`，也就是命令行界面，输入：

``` bash
shutdown /f /s
```

就可以了。
