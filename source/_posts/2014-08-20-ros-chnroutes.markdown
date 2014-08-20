---
layout: post
title: "RouterOS配置记录（二）：chnroutes"
date: 2014-08-20 16:52
comments: true
categories: network
---

之前写过一篇博客：Chnroutes的使用（Mac OS X Mavericks），讲述了在我的macbook pro上安装chnroutes，从而可以在上网的时候区分国内国外的IP，最终让国外的IP访问走VPN，国内的不走VPN。

[Chnroutes的使用（Mac OS X Mavericks）](http://markzhang.cn/blog/2013/12/04/chnroutes-on-mac/)

现在有了ROS，事情就变的更加棒了，把chnroutes做在ROS上，那所有的网络设备就自动具备了chnroutes的功能。所以在刚配置好了ROS之后，我就尝试配置了chnroutes。

<!-- more -->

首先我的配置参考了这篇文章：

[ros智能vpn自动翻墙](http://autorosvpn.blogspot.jp/2013/05/ros-vpn.html)

总的说来，在ROS上配置chnroutes，就是几个步骤：

1. 导入国内IP地址表
2. 对每个请求中的IP进行查找，如果不在国内IP地址列表中的，就标记一下
3. 在路由表中，根据标记，让数据走正常网关或是VPN

## 导入国内IP地址表 ##

这个其实非常简单，用VI就搞定了。首先根据我之前的文章：Chnroutes的使用（Mac OS X Mavericks），在我的MBP上运行 `./chnroutes.py -p mac`，就会生成ip-up/ip-down这两个文件。

ip-up文件里面就包含了所有的国内IP地址，所以接下来我们只需要将这个文件修改成ROS能认识的格式就行了，用VI就可以轻松搞定了。这里给出一个我目前在使用的（2014/7/22的数据）：

[2014/7/22 国内IP地址列表](/downloads/file/chinasites.rsc)

一般来说，这个列表不需要经常更新，在IPv4地址已经差不多消耗殆尽的今天，你能经常访问到新的属于国内的IP地址也不是件容易的事情。

接下来就是把这个 `chinasites.rsc` 上传到RouterBoard上就OK了，可以用Filezilla，使用FTP的方式。或者你如果是windows平台，使用WinBox的话，也可以用拖拽的方式将文件拖到ROS的 `Files` 对话框中即可。

上传了之后，点击ROS的 `New Terminal`，在命令行下输入：`import chinasites.rsc`，所有的国内IP地址列表就导入了，在ROS的 `IP` -> `Firewall` -> `Address Lists` 中就可以看到：

![China IP address list](/downloads/image/ros-ip-firewall-address-lists.png)

在上图中可以看到，我们给这些IP地址都设了一个名字叫做 `novpn`，表示路由这些地址的时候，不需要走VPN，后面会看到如何使用的。

## 标记Packets ##

这是Linux netfilter/iptables非常强大的功能体现了，首先来看在ROS里面是怎么配置的：

``` bash
Flags: X - disabled, I - invalid, D - dynamic 
 0   chain=prerouting action=mark-routing new-routing-mark=vpn passthrough=yes 
     dst-address=!192.168.88.0/24 dst-address-list=!novpn 
```

这个配置项位于：`IP` -> `Firewall` -> `Mangle`。首先这个配置针对的是 `prerouting` 表，对Mangle/Prerouting还不清楚的，看我的这篇文章：

[Linux Netfilter overview](http://markzhang.cn/blog/2014/07/23/netfilter-overview/)

然后，`dst-address-list=!novpn` 其实意思就是，如果目的地址不在 `novpn` 这个address list中的；最后 `action=mark-routing new-routing-mark=vpn` 就表示如果目的地址不在 `novpn` 中的，就给这些packet打上一个名为 `vpn` 的routing标记。

当然，上述命令中的：`dst-address=!192.168.88.0/24`，这表示本地局域网的地址不受该条规则影响，这也是非常好理解的。

所以总结来说，我们通过Linux netfilter的Mangle功能，成功的给不属于国内IP地址列表中的packet，打上了一个名为 `vpn` 的路由标记。

这里需要提一下 `passthrough=yes` 这个东西的作用。`passthrough=yes` 表示packet经过了本规则的处理之后，还会继续往下传递，给下面的rules继续处理。相反的例子是 `IP` -> `Firewall` -> `Fliter` 中的规则，比如 `accept` 或者 `drop` ，一旦匹配到了这些规则，就不会再继续往下处理了（下面的规则就不会再去执行了），所以这就是为什么我在上篇文章中提到说有一条 `drop` 的规则一定要放在最后。

所以这里我们设置了 `passthrough=yes` ，表示位于我们本条规则之后的规则还有机会继续被得到执行，所以你可以继续添加标记类的规则，或者其他任何Mangle的规则。所以再次重申，在Filter/Mangle这些表里，规则的顺序是很有影响的。

## 添加路由 ##

点击 `IP` -> `Routes`，在 `Routes` 标签页，点击加号按钮添加一条路由规则：

![ROS add routing](/downloads/image/ros-ip-routes.png)

- Dst. Address，这表示哪些目的IP地址受这条路由影响，这里填写 `0.0.0.0/0` 表示所有IP地址。

- Gateway，这表示这条路由走哪个Interface，一般来说，这就是你的VPN的PPP连接了。

- Distance，这表示这条路由的权重。在之前的一篇介绍ROS的基础设置的文章中，有说到我们添加了一个PPPoE client来实现宽带拨号，在创建那个PPPoE Client的时候，有一个选项叫做：`Default Route Distance`，我的这项设置是10，而如上图所示，现在我们添加的这条路由，我的设置是1。其实很好理解，我们在添加PPPoE Client的时候，有选择 `Add Default Route`，所谓Default route就是路由所有IP地址：`0.0.0.0/0`的，那么现在我们又添加了一条路由，也是路由所有IP地址的，那应该选哪条路由呢？这就是根据路由的Distance来决定的。Distance值越小的，权重越高。

- Routing Mark，这里填写的是 `vpn`。

所以总结来说，这条路由的作用就是，对于 `Routing Mark` 是 `vpn` 的packet，走我们指定的 `Gateway`。而且由于这条路由的权重高于默认路由，所以就实现了非国内的IP地址，走我们指定的Gateway，而国内的IP地址还是走默认路由（宽带连接的那条路由）。

写了一堆字，其实做一下就是两分钟的事。ROS的确是非常Power的东西。