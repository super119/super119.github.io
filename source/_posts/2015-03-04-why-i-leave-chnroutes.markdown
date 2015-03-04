---
layout: post
title: "RouterOS：为什么我不用chnroutes"
date: 2015-03-04 09:28
comments: true
categories: network
---

之前有一篇文章说了在RouterOS下配置chnroutes：

[RouterOS配置记录（二）：chnroutes](http://markzhang.cn/blog/2014/08/20/ros-chnroutes/)

配置了chnroutes之后，国内IP不走VPN，国外的走VPN，一切看起来都很好。但是随着使用时间的增长，发现了一些问题：

<!-- more -->

- CDN的问题。由于翻墙的需要，所以我们不会使用ISP给我们提供的DNS server。一般来说，我们都会使用诸如 `8.8.8.8` 或者OpenDNS来作为我们的DNS server。但是由于这些地址本身就是国外地址，所以当访问这些DNS server的时候，根据chnroutes配置的策略，就会走VPN走。这样一来，对于你要访问的网站来说，它会认为你是一个国外的IP，从而无法给你正确的你想要的一个CDN IP。这就是为什么有的时候访问国内的一些在线直播的网站，挂上VPN和chnroutes就会卡，拿掉立马就好了的原因。

- VPN的主要作用是用来访问被墙的网站，一般来说VPN本身的带宽都不会特别大（除非你很有钱买高档VPN），所以其实对于一些没有被墙的国外网站，不使用VPN其实更加快捷。

- PSN（PlayStation Network，现在叫Sony Entertainment Network）无法登录，一些游戏在线搜索不到房间。具体原因我还不清楚，我的感觉可能是PSN的服务器和认证服务器不是同一个IP，而且一个是国内IP一个是国外IP。可能认证服务器需要验证客户端的IP地址，而认证服务器拿到的客户端的IP并不是之前我们连接PSN服务器的那个IP，这样导致了混乱。当然这是纯YY，很可能是其他原因。总之就是PSN无法登录，有一些游戏的在线联机也没法工作。

基于此，我在想我是否真的需要chnroutes。现在想来其实我并不需要，因为我的需求基本上就只有一条：访问Google，包括gmail, google gerrit, plus, drive...

所以我开始构思不使用chnroutes的另外一条道路。

最一开始没想彻底抛弃chnroutes，就想先解决第一个问题，对于我来说，这个问题是最关键的。所以对于这个问题，很自然的就想到了 -- 把DNS server的IP也加到chnroutes的地址表里面，假装成国内的IP不就OK了嘛... 试验了一下发现无效，因为这样一来，DNS的请求就会经过墙，DNS劫持就发生了，现象就是你访问google/youtube，出来的是另外一个网站，或者干脆就是连接不上。

有朋友建议我用TCP DNS，这样可以解决墙的DNS劫持的问题。但是这也不好做，因为我是配置ROS作为整个家里局域网的DNS server的，ROS本身是不支持TCP DNS的，为此有人（也是中国小伙）还在Mikrotik的论坛上发过类似的帖子：

[How to force DNS requests using tcp protocol!](http://forum.mikrotik.com/viewtopic.php?t=68248)

其次，如果不使用ROS作为DNS server，也就是说让局域网内每台设备手动配置DNS（比如配置成OpenDNS的IP），这样貌似可行但其实也非常麻烦。因为TCP DNS毕竟是非常规需求，而且现在的设备早已不局限于电脑，还包括了手机、pad甚至各种嵌入式设备（比如树莓派、存储服务器等）。这些东西能否支持TCP DNS也是一个问题。

接下来我想到了autoproxy gfwlist：

[https://code.google.com/p/autoproxy-gfwlist/](https://code.google.com/p/autoproxy-gfwlist/)

这张列表包含了天朝人民发现的被墙的网址和IP。我的想法很简单，既然chnroutes的IP列表太宽泛，那就找一张精准的表好了。结果把autoproxy这张表拿出来一看，傻了。因为这张表中包含了诸如：`*.google.com` 这样的网址，这样的东西在ROS里面如何处理？因为ROS作为路由设备，是只认识IP地址的，对域名它可是没有办法的。

虽然ROS的确是有一个叫做 `WebProxy` 的功能，但是通读了这个功能的manual之后，你会发现你只能针对域名设置 `Allow` 和 `Deny`，没有更多的动作可以做了。

如果你Google `routeros autoproxy gfwlist`，你会发现github上有一个项目叫 `gfwlist2routeros`：

[https://github.com/layerssss/gfwlist2routeos](https://github.com/layerssss/gfwlist2routeos)

显然这也是国内某位大神的作品。但是就在项目的README中，也写到了：

> gfwlist 中包含一些通配符规则，如 *.google.com.* 之类的，该项目无法识别

再次受挫感觉心灰意冷。在决定要放弃之前想到了最后一个笨办法。因为我的需求相对简单，主要就是要使用Google，所以我想我就自己建一个网站list，然后在路由器上写一个脚本，定时去解析这些网站然后将IP地址储存下来。这样当脚本运行了一定次数之后，我想可能绝大部分Google的这些IP就都能采集到，然后设置这些IP地址走VPN即可。

花了几个小时写完了脚本，然后设置了定时执行，睡了一觉之后起来之后测试发现还是不行。现象还是一样，要不就是网站被劫持了要不就是连不上。

分析了一下大致是这么几个原因：

- 脚本在ROS上run，做DNS解析的时候也必须要从VPN走，否则就会被墙劫持。虽然我对OpenDNS的IP做了处理，但是我发现那些规则是在Firewall里面做的，对于ROS来说，它是不通过Firewall的...

- 修改了脚本，脚本一开始，将OpenDNS的IP加入到了静态路由中，解析完成后删除，发现还是不行。后来一想，局域网中有很多设备都是开着的，只要这些设备一访问Google，就会在ROS的DNS server中产生cache，这样我的脚本取到的IP地址就不正确了，因为这些地址都是墙参与产生的...

到此彻底没了脾气。休息了一天之后发现ROS的DNS有静态DNS的功能（`/ip dns static`），于是又想到了可能可以对上面的方案做一点改进来达到目的。最终目的达成测试OK，具体来说我的做法是这样的：

- 首先我写了一个脚本，在我的VPS上定时运行。这个脚本非常简单，就是去解析我给定的一个网站list，解析出每个网站的IP地址然后存成文件。将脚本放在VPS上执行而不是在ROS上执行的好处就是：彻底回避了墙。而且最后这些IP地址要从VPS走，所以在VPS上做DNS解析是最正确的做法了。脚本是这样的：

``` bash
#!/bin/bash
HOSTS=(
	'www.google.com'
	'www.googleapis.com'
	'www.gstatic.com'
	'mail.google.com'
	'accounts.google.com'
	'plus.google.com'
	'play.google.com'
	'www.gmail.com'
	'drive.google.com'
)

echo -n "" > /home/mark/vpn-hosts-ips

for i in ${HOSTS[@]}; do
	ip=`ping -c 1 ${i} |grep PING|cut -d " " -f3|tail -c +2|head -c -2`
	echo ${ip} >> /home/mark/vpn-hosts-ips
done

cp /home/mark/vpn-hosts-ips /Public/pub
chmod 644 /Public/pub/vpn-hosts-ips
```

使用Cron将这个脚本定时执行。脚本的最后将生成的文件放到了 `/Public/pub` 目录下，这是一个FTP的目录，作用就是接下来ROS可以下载到这个文件。

- 在ROS上写一个脚本，这个脚本也是定时执行。脚本会通过FTP，去download上面步骤中生成的IP地址列表文件，然后将这些IP地址添加到静态DNS中，同时也将这些IP添加到一个address list中。这样通过 `/ip firewall mangle` 和路由表的设定，设置好这些IP地址从VPN走。ROS上的脚本是这个样子的：

``` bash
:global hosts {\
"www.google.com";\
"www.googleapis.com";\
"www.gstatic.com";\
"mail.google.com";\
"accounts.google.com";\
"plus.google.com";\
"play.google.com";\
"www.gmail.com";\
"drive.google.com";};

/tool fetch mode=ftp url="ftp://<vps ip>/pub/vpn-hosts-ips"

:global content [/file get [/file find name=vpn-hosts-ips] contents]
:global contentLen [ :len $content ]
:global lineEnd 0
:global line ""
:global lastEnd 0
:global index 0

/ip dns static remove [/ip dns static find]
/ip firewall address-list remove [/ip firewall address-list find]

:do {
    :set lineEnd [:find $content "\n" $lastEnd]
    :set line [:pick $content $lastEnd $lineEnd]
    :set lastEnd ( $lineEnd + 1 )

    /ip dns static add address=$line name=[:pick $hosts $index]

    :local found false
    :foreach j in=[/ip firewall address-list find address=$line] do={
        :set found true;
    }

    :if ($found = false) do={
        /ip firewall address-list add list="vpn-list" address=$line
    }
    
    :set index ( $index + 1 )
} while ($lineEnd < $contentLen)

/ip dns cache flush
```

这样一来结果基本上满意了，测试访问google都一切正常。而且在ROS上，我也不再需要使用任何其他DNS了，使用ISP提供的DNS就可以了。

唯一的缺点就是，我们需要手动维护一张网站list，所以如果要翻墙的网站很多的话，这个方法是不适合的。我想如果谁有一个比较全的常用的Google的site list的话，不妨共享一下。
