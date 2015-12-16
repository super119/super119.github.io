---
layout: post
title: "Raspberry Pi + ArchLinux：网络dhcp不稳定，经常无法获取IP地址"
date: 2015-12-16 14:40
comments: true
categories: linux
---

经常发现树莓派开机之后网络没有起来，最终发现是因为dhcpcd的-t参数，也就是等待dhcp server响应的timeout时间太短导致的。

默认是30秒，但是ArchLinux中配置的是10秒。

查看了脚本，调用dhcpcd的是netcfg，netcfg本身也是一个脚本，所以最终发现这个-t是在/usr/lib/network/connections/ethernet中设置的。

于是在这个文件的开头加上了：`DHCP_TIMEOUT=300`

这样超时就变成了5分钟，足够了。 
