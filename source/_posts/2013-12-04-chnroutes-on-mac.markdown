---
layout: post
title: "Chnroutes的使用（Mac OS X Mavericks）"
date: 2013-12-04 14:00
comments: true
categories: network
---

翻墙是我朝人民的普遍需求。现在翻墙的普遍做法是搞一个VPN。买一个也好，找免费的也好。有了VPN之后面临的问题就是当VPN连接之后，所有的网络数据都会走VPN，这显然不是最高效的做法，对于一些没有被墙的网站或是应用，没有必要再走VPN，这样反而更慢。为此，`chnroutes`应蕴而生了。

<!-- more -->

`chnroutes`就是一份路由表配置，它收集了绝大部分国内没有被墙的网站/IP，然后将他们加到路由表中，设定访问这些IP地址使用原来的网关（非VPN）。这样，在路由表中的IP地址就会走原来的网关，剩下来的就都走VPN了。

在`https://code.google.com/p/chnroutes/`上下载`chnroutes.py`，在Mac OS X上运行`./chnroutes.py -p mac`就得到两个文件：`ip-up` 和 `ip-down`。

打开文件一看就明白，没比这更简单的脚本了。`ip-up`就是使用netstat等命令取到非VPN的网关的IP地址，然后就调用`route`命令设置一大堆的路由信息。

这里要注意的是，在我的Macbook上（Mac OS X Mavericks），`ip-up`取到的所谓`OLDGW`是不对的。`ip-up`试图去取非VPN的那个网关IP地址，但其实取到的是我所有的网关，也就是包含了VPN的和本地的两个网关IP地址。所以我注释掉了`ip-up`中获取`OLDGW`的一行，直接将`OLDGW`设置成了我本地的网关IP地址。

这样，连接上VPN，然后运行`ip-up`就可以舒服的上网了。运行`ip-down`就可以删除掉`ip-up`添加的路由信息。

为了验证`ip-up`的确做了我们所想的事情，可以用命令`traceroute`来验证，比如：`traceroute www.youku.com`，就可以看到数据是从本地网关走的，而如果是：`traceroute www.youtube.com`，就可以看到数据是从VPN走的。
