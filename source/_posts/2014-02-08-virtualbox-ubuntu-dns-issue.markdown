---
layout: post
title: "VirtualBox中安装Ubuntu，在有VPN的情况下会有DNS的问题"
date: 2014-02-08 21:11
comments: true
categories: linux
---

标题表达的不是很清楚（否则标题就太长了。。。），意思是：如果我们在一台windows的机器上安装了VirtualBox，然后在里面安装Ubuntu，然后在windows中连上VPN，此时Ubuntu虚拟机中就会有DNS的问题。我测试的是最新版本的VirtualBox，Ubuntu是12.04的版本，不知道将来这个问题会不会解决。

<!-- more -->

发现这个问题是因为我使用的VPN是可以翻墙的，举例来说可以访问到youtube，但是我发现在我的Ubuntu虚拟机中就无法访问youtube和其他一些我需要的网站。而且我发现Ubuntu中，浏览器的现象就是DNS解析都完成不了，所以就一直无法打开。

经过Google，在这里发现了答案：

[http://superuser.com/questions/570984/virtualbox-guest-ubuntu-loses-dns-when-host-connects-to-vpn](http://superuser.com/questions/570984/virtualbox-guest-ubuntu-loses-dns-when-host-connects-to-vpn)

简单来说，你只需要这样做：

1. 关闭虚拟机
2. 在Windows中打开cmd.exe，然后输入：

```
C:\Program Files\Oracle\VirtualBox\VBoxManage.exe modifyvm "ubuntu" --natdnshostresolver1 on
```

命令中的`ubuntu`是我的ubuntu虚拟机的名字（VM name）。

上述链接中，作者也不太明白为什么，他是这么说的：

> I suspect it's because when the VPN is active, the host is doing something special for DNS lookups besides just forwarding requests to the specified DNS servers that VirtualBox picked up from the Windows config.