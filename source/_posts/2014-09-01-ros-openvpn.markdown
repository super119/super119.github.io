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

OpenVPN server侧的配置稍微复杂一些，关键是要生成一堆证书、秘钥。但是也不是复杂到没法忍，关键是要有一篇好的turtorial。所以这里是我找到的一篇很好的文章，照做就行：

[Secure communications with OpenVPN on Ubuntu 12.04](https://www.linode.com/docs/networking/vpn/secure-communications-with-openvpn-on-ubuntu-12-04-precise-and-debian-7)

照着上面的文章做就可以了，这里再给出我最终的OpenVPN server的配置文件作为一个参考：

``` bash
port 3389
proto tcp
dev tun

#Keys
ca /etc/openvpn/easy-rsa/2.0/keys/ca.crt
cert /etc/openvpn/easy-rsa/2.0/keys/server.crt
key /etc/openvpn/easy-rsa/2.0/keys/server.key
dh /etc/openvpn/easy-rsa/2.0/keys/dh1024.pem

# This will be the internal tun0 connection IP
server 10.8.0.0 255.255.255.0
ifconfig-pool-persist ipp.txt

# This will send all of a client's 10.x.x.x traffic through the VPN
# push "route 10.0.0.0 255.0.0.0"
keepalive 10 120

# Mikrotik does not support LZO compression
#comp-lzo

auth SHA1
# Select a cryptographic cipher.
#cipher BF-CBC        # Blowfish (default)
cipher AES-256-CBC

#user nobody
#group nogroup
status /var/log/openvpn/openvpn-status.log

persist-key
persist-tun
verb 3
```

几点解释：

1. ROS的OpenVPN client只支持TCP，所以这里使用TCP模式：`proto tcp`。
2. 被注释的那句：`push "route 10.0.0.0 255.0.0.0"` 是可以用来给客户端设置路由表的，但是由于我的ROS有chnroutes，所以我会在我的ROS上设置路由表，所以我的OpenVPN server侧就没有push路由表给客户端了。
3. `comp-lzo` 一定要注释掉，这是因为ROS OpenVPN client不支持这个。所以ROS的OpenVPN UDP不支持，数据压缩又不支持，你就可以想想Mikrotik论坛上大家的怨气有多大了。有空可以上去搜看相关的帖子，相当欢乐。
4. 有关cipher，Blowfish, AES这些ROS OpenVPN client都是支持的，所以可以自由选择。使用默认的Blowfish就挺好。
5. 最后的 `verb 3` 可以改成诸如 `verb 6`，这样在 `/var/log/syslog` 中可以得到很多的log，客户端连不上OpenVPN server的时候，可以帮助调试和定位。

配置完成之后，`sudo service openvpn restart`，OpenVPN server就ready了。

## OpenVPN client侧配置（ROS） ##

相比server侧，ROS上client的配置就相对简单了很多。

首先是上传证书和秘钥到ROS上：

![Certificates](/downloads/image/ros-system-certificates.png)

这里其实是3个文件，client的公钥和私钥，还有就是server的证书，这三个文件首先upload到ROS上，然后在上图中点击 `Import` 按钮就可以导入了。

然后就是创建一个OpenVPN的client了：

![PPP Create OpenVPN Client](/downloads/image/ros-ppp-openvpn.png)

几点说明：

1. `Connect To` 和 `Port` 不用说了
2. `Mode` ，这个有两个选项：`IP` 和 `Ethernet`，这表示数据是走layer 3还是layer 2，反映到VPN创建出来的本地虚拟网卡，就是tun还是tap，一般来说OpenVPN都是tun，所以这里是选择 `IP`。如果你使用PPTP，就可以看到虚拟网卡是tap。
3. `User`，这个其实不需要，因为OpenVPN是使用证书来识别client的，每个client都会有一个证书。但是这里ROS需要填一个用户名，所以随便填一个就好（貌似OpenVPN也可以支持username/password的那种验证的，我没去调查）。
4. `Profile`，因为在下面会选择加密方式等，所以profile就选default就好，default意思就是按照server的默认设置来。
5. `Certificate`，这里要选择你upload到ROS上的，client的私钥，这个私钥是用来解密数据的。
6. `Auth` / `Cipher`，这个就选成跟server端的配置一致即可。

## 结束 ##

所以总的来说，OpenVPN的配置并不复杂，只是ROS不支持UDP的OpenVPN，所以用来翻墙看看网页还是可以的，玩游戏啥的就要差不少。不过OpenVPN作为一个free的，性能优良的VPN solution，在国内网络中使用也是相当不错的。

