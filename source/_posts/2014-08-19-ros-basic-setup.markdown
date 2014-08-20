---
layout: post
title: "RouterOS 配置记录（一）：基础设置"
date: 2014-08-19 14:38
comments: true
categories: network
---

RouterOS（以下简称ROS）是一个软路由系统，由拉脱维亚的Mikrotik开发，完全基于Linux。项目刚开始的时候，只支持x86，所以我们可以找一台PC机来安装ROS，从而可以让PC机瞬间变身成一个软路由。

后来Mikrotik开发了专用的硬件设备：RouterBoard。RouterBoard是MIPS架构，运行ROS，所以不想搞一台PC机的话，买一台RouterBoard就显的非常不错。而且RouterBoard有好多型号，带wifi的不带wifi的，企业用的个人用的，千兆的百兆的。。。，最关键的是家用的RouterBoard，带wifi的也不贵，所以最近就把家里的路由器换成了RouterBoard，型号是：951Ui-2HnD。

本系列文章旨在记录我在使用ROS的时候的一些配置，之前已经写了一篇有关修改WinBox字体的文章，算是个引子。ROS虽然强大，但是配置项目的繁杂也颇为吓人，坦白说没有网络基础的话还是别用ROS。

<!-- more -->

本文讲述拿到RouterBoard之后的最基础配置，也就是配置宽带拨号，WIFI，NAT，DHCP，最终能让家里的各网络设备上网。

## 宽带拨号 ##

RouterBoard第一次开机，winbox连入之后，会有一个提示让我们备份当前的配置或是删掉目前的配置，为了从头开始，我删掉了原来的所有配置，从头开始一步一步自己来。

配置宽带拨号非常简单，就是在 `PPP` 里面添加一个 `PPPoE Client` 的interface即可：

![ROS add PPPoE client](/downloads/image/ros-pppoe-client.png)

必须要填的会以红色表示，在上图中，要填的是Name和Interfaces。Interfaces指的是哪个网口连接了宽带网线，一般来说，RouterBoard的第一个网口我们都用来连接外网（WAN），所以选择第一个网口即可（如上图所示，我给第一个网口做了重命名，修改名称为ether1-wan）。在第二个标签页 `Dial Out` 中，就需要填写你的宽带账号和密码了，此外 `Use Peer DNS` 和 `Add Default Router` 也可以勾选上。

`Use Peer DNS` 指的是是否使用宽带运营商提供的DNS（在PPPoE拨号的时候能得到），一般都需要使用，当然你也可以不用运营商提供的DNS，自己定义一些DNS server，比如Google的 `8.8.8.8`, `8.8.4.4`，还有OpenDNS的 `208.67.222.222`, `208.67.220.220`。只是使用这些DNS的话，你要先把翻墙准备好。

在Winbox的 `IP` -> `DNS` 里面，就可以自己设置DNS server：

![ROS DNS setting](/downloads/image/ros-ip-dns.png)

在上图中可以看到，`Dynamic Servers` 就是来自宽带运营商的DNS server信息。

而 `Allow Remote Requests` 指的是，RouterBoard本身是否也作为一个DNS server从而可以响应DNS查询请求。一般来说，这个应该enable。顺理成章的，下面的一些有关Cache的配置也就变的有意义了，如上图所示，我加大了Cache的size，从而可以多缓存一些DNS信息。不过在enable了 `Allow Remote Request` 之后，需要注意要打开TCP/UDP的DNS端口，以防客户端无法通过RouterBoard查询DNS。

回到宽带拨号，配置好了 `PPPoE Client` 之后，就可以尝试去enable它了，一旦连接成功，就表示你的宽带拨号已经OK了。

## WIFI ##

打开 `Wireless` ，在Interface标签页，可以看到一个wireless interface，默认名字是wlan1，关键的配置在标签页 `Wireless`：

![ROS Wirelss config](/downloads/image/ros-wireless.png)

大部分都非常好理解，有几个地方需要注意：

- Channel Width，这里有所谓的HT Above和HT Below，有关这个的解释是（来自Mikrotik官方）：

> ht above and ht below allows to use additional 20MHz extension channel and if it should be located below or above control (main) channel. Extension channel allows 11n device to use 40MHz of spectrum in total thus increasing max throughput.

不过需要注意的是，不是所有的Frequency都可以设置Above/Below，这里有一张来自Mikrotik的表说明了这一点，所以最终在设置Frequency的时候需要注意：

![ROS wireless HT above/below setting](/downloads/image/ros-wireless-ht-above-below.png)

- Frequency，上面已经提到了，设置Frequency要注意HT Above/Below是否有取值，此外可以通过点击 `Scan` 按钮来查看当前环境中有哪些Frequency是被占用的，尽量挑选一个没人使用的频率吧。

- Wireless Protocol，Mikrotik开发了一些自己私有的WIFI协议，比如这里看到的NV2等，对于我们来说，肯定是使用802.11了。

- Security Profile，这个后面紧接着就要配置，就是在这个profile中，我们可以配置WIFI的加密方式（WPA/WEP等）。

- Bridge Mode，要选成Enabled，后面会提到，为了让WIFI的clients能和RouterBoard上的几个以太网口通信，需要将它们都放到一个bridge中。

接下来就要配置WIFI Security Profile了，如下图所示：

![ROS wireless security profile](/downloads/image/ros-wireless-security-profile.png)

在 `Security Profile` 标签页中，点击加号按钮就可以新建一个 `Security Profile` 了。上图中我们配置的security profile name是default，所以在刚才我们设置Wireless Interface的时候，`Security Profile` 一项我们选择的就是这个 `default`。

配置到这里，WIFI热点就配置完成了，接下来我们需要将WIFI的wlan1这个interface和以太网的四个端口（第一个端口ether1-wan我们是分配给了宽带连接）放到一个bridge中去，这样才可以实现WIFI的clients和这四个以太网口互通。

在配置Bridge的时候，其实有两个配置步骤，顾名思义Bridge肯定只能包含两个Interface（Bridge的两端），而我们需要将wlan1和四个以太网口都配置到Bridge上去，那怎么办？

所以第一步是，将以太网口3/4/5都配置成以太网口2的 `Slave Port`，如下图所示：

![ROS interface master port](/downloads/image/ros-interface-master-port.png)

点击 `Interfaces`，在出来的对话框的 `Interface` 标签页，点击以太网口3/4/5就可以看到上图。在上图中可以看到，我们将以太网口3（ether3）的 `Master Port` 这一项选成了 `lan-ps3`。这里解释一下，`lan-ps3` 就是我的第二个以太网口，因为这个网口接着我的PS3，所以我给它取名为 `lan-ps3`。

将3/4/5以太网口设置成以太网口2的Slave Port之后，就可以创建Bridge了，点击 `Bridge`，在出来的对话框的 `Bridge` 标签页中，点击加号新建一个Bridge，啥都不需要修改，只需要给个名字就OK了。关键是在标签页 `Ports` 中，点击加号按钮，将wlan1和lan-ps3加入到Bridge中去：

![ROS Bridge ports](/downloads/image/ros-bridge-ports.png)

这样就OK了，WIFI部分的配置就差不多了。

## NAT ##

但凡搞过Linux iptables的都知道了，input/forward/output，pre-route/post-route...，对这些概念不清楚的可以看我这篇文章：

[Linux Netfilter overview](http://markzhang.cn/blog/2014/07/23/netfilter-overview/)

说起来配置一个简单的能上网的NAT非常的简单，选择 `IP` -> `Firewall`，在 `NAT` 标签页里面点击加号按钮，添加一个NAT：

![ROS add NAT](/downloads/image/ros-ip-firewall-nat.png)

总共就只需要做三件事情：Chain选成srcnat（表示伪装源地址），Out Interface选成宽带连接那个PPPoE client，最后在 `Action` 标签页里面选择动作为：masquerade。

这样就表示所有的packet，在经过路由表之后，如果最终经过路由决定要从 `Out Interface` 这个接口出去的话，就做masquerade。

## DHCP Server ##

- 首先是创建一个IP Pool。点击 `IP` -> `Pool`，然后新建一个Pool：

![ROS create IP pool](/downloads/image/ros-ip-pool.png)

就是填写一个名字，一个IP地址范围即可，比如我这里设置的就是192.168.88.2-192.168.88.254，RouterBoard自己就是192.168.88.1了。

- 然后就是创建一个DHCP server了，来到 `IP` -> `DHCP Server`，点击加号按钮创建一个DHCP Server:

![ROS Create DHCP server](/downloads/image/ros-ip-dhcp-server.png)

需要注意的就是，首先 `Interface` 那一项要选成你的Bridge device，这样WIFI和以太网的网络client就都可以DHCP了；其次 `Address Pool` 那一项就选我们刚才创建的那个IP Pool。

- 最后设定DHCP Server的Network的一些信息，还是在DHCP Server的对话框中，选 `Networks` 那个标签页，点击加号按钮：

![ROS DHCP server networks](/downloads/image/ros-ip-dhcp-server-networks.png)

可以看到，这其实就是设定网关和DNS Server的。上图中，对于192.168.88.0/24这个网络的DHCP，clients自动获得了IP地址之后，获得的网关就是192.168.88.1，DNS也是192.168.88.1。联想到一开始的时候提到的 `IP` -> `DNS` 的设定，网络客户端知道DNS Server是RouterBoard就是在这里设定的。

## 最后 ##

最后我们还可以配置一些简单的防火墙规则，用来拦截来自外网的请求，这里用命令行的方式来展现，这些规则都是配置在：`IP` -> `Firewall` -> `Filter` 里面的，非常简单：

``` bash
Flags: X - disabled, I - invalid, D - dynamic 
 0   chain=input action=accept protocol=icmp 

 1   chain=input action=accept connection-state=established 

 2   chain=input action=accept connection-state=related 

 3   chain=input action=accept protocol=tcp dst-port=22 

 4   chain=input action=accept protocol=tcp dst-port=8291 

 5   chain=input action=accept protocol=tcp dst-port=3987 

 6   chain=input action=accept protocol=tcp dst-port=80 

 7  chain=input action=accept protocol=tcp dst-port=53 

 8   chain=input action=accept protocol=udp dst-port=53 

 9   chain=input action=drop in-interface=SHUnicom 
```

打开22端口是为了可以远程ssh RouterBoard，打开8291/3987/80端口是为了可以远程使用WinBox登录（可以额外添加src-address来限定只允许某个IP地址登录，这样可以增强安全性），接受53端口是为了DNS查询（实际上DNS一般都是使用UDP，TCP不需要打开），最后一条就是拦截所有外部的网络请求，请确保这一条一定位于所有规则的最后，iptables是匹配到一条规则之后就会返回的，所以如果这条不出现在最后，那么这一条之后的所有规则都会无法生效。

这个最后一条规则，可以看到，是说如果是从 `SHUnicom` 这个接口进来的数据一律drop，而 `SHUnicom` 是我的宽带连接接口。想到这个宽带连接其实是在我的ether1-wan这个物理接口上，那么这里是否也可以设置成从 `ether1-wan` 接口进来的数据一律drop呢？仔细想想是不对的，`ether1-wan` 是物理接口，用于宽带拨号；而 `SHUnicom` 其实是一个PPPoE，它会创建一个虚拟的网卡interface，所以我们要drop的是从这个虚拟的网卡interface尝试进入RouterBoard的数据，而不是物理的 `ether1-wan` 上进入的数据。如果我们真的drop了 `ether1-wan` 上进来的数据，那就上不了网了，因为所有的返回的数据都被丢掉了。在PPPoE/VPN的时候，虚拟网卡会被创建，所以这个时候要分清楚虚拟网卡和物理网卡，免的设置错误的规则。

能上网只是最基本需求了，接下来才是开始各种折腾的时候，VPN/chnroutes/DDNS/... 后续博客将记录这些。
