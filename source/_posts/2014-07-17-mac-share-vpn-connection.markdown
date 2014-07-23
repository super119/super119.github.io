---
layout: post
title: "Mac OS X（Mavericks）共享VPN给其他设备"
date: 2014-07-17 11:22
comments: true
categories: MacOSX network
---

之前写过一篇文章叫“Chnroutes的使用“，主要说的是如何通过使用 `chnroutes` 从而将国内和国外的路由分开，国内走正常路由，国外则走VPN。现在的问题是，如果在一台电脑上有了VPN之后，如何将这个VPN share给其他设备，比如PS3？

<!-- more -->

首先很自然想到的是Mac OS X（Mavericks）自带的 `Internet Sharing` 功能：

![Mac OS X Internet Sharing](/downloads/image/mac-internet-sharing.png)

但是通过使用后发现有如下的问题：

1. 你需要有两个网络设备，才能开始share。上面的截图也可以看到，你要选择一个 `From` 的设备，再选择 `To` 的网络设备（也就是说其他设备是通过本机的这个网络设备连接的）。

2. 当然你可以说，VPN在Mac OS X里面也是一个单独的网络设备啊。是，但是通过试验，如果你只有一个网络设备，比如WIFI，那么就意味着我们既需要通过这个WIFI去连接VPN，还要通过这个WIFI让其他的设备能够连到本机，这是没法做到的。试验的结果就是，当你开了 `Internet Sharing` 之后，你的VPN就拨不上去（一拨就掉）；当你开了VPN之后，`Inetnet Sharing` 一开你的VPN就掉。事实上，如果通过WIFI来share的话，你会发现Mac OS X会将WIFI配置成一个AP，所以自然一旦被enable，你的VPN就掉了。

3. 就算以上两点对你来说都没问题，这第三点就是绝望的来源：Mac OS X不会forward VPN的数据。也就是说，你想要通过Mac OS X的这个 `Internet Sharing` 功能来share你的VPN的话，是不可行的。

所以没办法还是通过万能的Google来寻找答案，答案在这里：

[http://rodrigo.sharpcube.com/2010/06/20/using-and-sharing-a-vpn-connection-on-your-mac/](http://rodrigo.sharpcube.com/2010/06/20/using-and-sharing-a-vpn-connection-on-your-mac/)

基本上就是需要我们写脚本来实现，上面这篇文章中，VPN使用的是OpenVPN，我测试的时候，使用的是PPTP的VPN，所以根据上文中 `Troubleshooting` 一节的描述，我做了这么两个脚本：

{% include_code 开启VPN共享 bash share-pptp-vpn.sh %}

{% include_code 关闭VPN共享 bash off-share-pptp-vpn.sh %}

所以当拨上了VPN之后，运行开启VPN共享那个脚本就OK了。在PS3上将网关指到Macbook的IP地址就可以了。

最后一个遗留问题。Mac OS X的VPN高级设定中有这么一个checkbox：

![Send all traffic over VPN](/downloads/image/mac-send-traffic-over-vpn.png)

这个 `Send all traffic over VPN` 我不太理解是什么意思，是指本地NAT网络的地址不走VPN么？还是Mac OS X内置了类似 `chnroutes` 这样的功能？

有意思的是，有的时候如果enable了这个选项，我发现PS3连接网络就会出现问题。而如果VPN已经拨上去了，再去enable这个选项貌似就没有影响。

没有深究这个，所以这是目前的一个遗留问题。
