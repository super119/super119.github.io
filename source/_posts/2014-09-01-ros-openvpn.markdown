---
layout: post
title: "RouterOS配置记录（四）：OpenVPN Client"
date: 2014-09-01 15:42
comments: true
categories: network
---

OpenVPN是现在使用相当广泛的一种VPN solution。它不使用特定协议（比如PPTP依赖于GRE协议，如果GRE协议被墙，PPTP就完了），也不使用特定端口，TCP/UDP都支持，性能表现也相当优异（UDP）。所以现在使用OpenVPN作为VPN的solution很多，在Mikrotik的论坛上可以找到大量赞美OpenVPN的帖子。

但是，事实上当我把OpenVPN全部配置完成之后，发现两个问题：

<!-- more -->

- OpenVPN已被墙，应该就是2013年（或者更早）的事情。虽然上面提到了OpenVPN不使用特定协议，不使用特定端口，但是聪明的GFW还是找到了OpenVPN TLS验证时的特征并墙之。所以如果你辛辛苦苦配置好了服务器端和客户端，客户端拨号的时候发现："Peer disconnected/Connection reset"，那就是被墙的标志。OpenVPN TLS验证的时候，连接直接被reset了。

针对这个问题，当然也有解决方案，那就是模糊TLS验证的数据，让GFW寻不到TLS的特征，比如：

[http://igfw.net/archives/13018](http://igfw.net/archives/13018)

但是这种做法是没法在ROS上实现的。因为这要求修改OpenVPN sever和client的代码，而我们是无法修改ROS中的OpenVPN的实现的。

- ROS不支持UDP模式的OpenVPN。这个已经被广大ROS用户诟病已久，但不知道为什么Mikrotik就是不改：

[http://forum.mikrotik.com/viewtopic.php?f=1&t=26499](http://forum.mikrotik.com/viewtopic.php?f=1&t=26499)

TCP模式的OpenVPN延迟（latency）很高，不适合诸如电话会议、在线游戏等场景。

所以坦白说，有上述的两个问题，ROS+OpenVPN，已经没有太多的使用价值了。但是既然我已经将其配置完成了，所以本文还是将过程记录了下来。

## OpenVPN server侧配置 ##



## OpenVPN client侧配置（ROS） ##


