---
layout: post
title: "在Ubuntu上连接Juniper SSL VPN"
date: 2014-03-20 22:26
comments: true
categories: linux network
---

现在国内很多外企都使用Juniper SSL VPN，来让员工可以在家连入到公司的网络。但是我发现，Juniper的SSL VPN在windows和Mac OS X上都可以很好的工作，但是在Ubuntu上貌似就有问题。就算你正确安装了Java Runtime，打开浏览器，`Host Checker` 和 `Network Connect` 也无法正常运行。有人说这是因为现在我们大多使用64位的Ubuntu，而 `Host Checker` 和 `Network Connect` 需要32位的浏览器和32位的JRE才能工作。

我没有去深究32位的Ubuntu下是否Juniper VPN就真的可以正常工作了，因为我找到了一个更简单的办法。

<!-- more -->

这个方法来自这位大神，他自己分析了Juniper SSL VPN的工作过程，然后写了一个perl脚本解决了这个问题：

[http://smallhacks.wordpress.com/2012/07/15/jvpn-perl-script-to-connect-to-the-juniper-vpn-with-host-checker-enabled/](http://smallhacks.wordpress.com/2012/07/15/jvpn-perl-script-to-connect-to-the-juniper-vpn-with-host-checker-enabled/)

对原理感兴趣的可以自己去看这篇文章（可能需要翻墙），这里简述一下步骤：

- 从上面的文章提供的地址，下载 `jvpn-0.7.0.tar.bz2`。

- 解开，将配置文件 `jvpn.ini` 拷贝一份以便输入自己的配置，比如：`cp jvpn.ini mark.ini`，然后编辑这些配置项：

``` bash
host: 你的VPN server的地址
username: 用户名
realm: 区域，一般的外企应该是 `Employee` 或者 `Contractor`
verifycert: 是否验证SSL证书，如果你本地没有导入你的VPN server的SSL的证书的，可以写0，否则写1
dnsprotect: 这可以防止Ubuntu的NetworkManager修改 `/etc/resolv.conf` 。我们需要修改这个文件从而修改DNS server的配置。
mode: `ncsvc/ncui` 都可以
```

- 然后就可以执行：`sudo perl ./jvpn.pl --conf mark.ini`。成功了就可以看到连到了VPN，按 `Ctrl+C` 可以中断连接。

** Troubleshooting **

- `sudo apt-get install libterm-readkey-perl`，如果jvpn.pl说 `Term::ReadKey module` 找不到的话
- `sudo apt-get install lib32z1`，如果出现这样的错误： `libz.so.1: cannot open shared object file: No such file or directory`
- `sudo apt-get install gcc-multilib`，如果看到这样的错误： `bits/predefs.h: No such file or directory`
- `sudo apt-get install libhttp-request-ascgi-perl`，如果jvpn.pl说 `HTTP::Request module` 找不到的话

这样在Ubuntu下，就可以使用Juniper SSL VPN了。很多外企的网络，自带翻墙，所以拨上VPN之后，除了工作之外，还可以实现自动翻墙。配合上chnroute，就完美了。不清楚chnroute有啥作用的，可以看我之前写的一篇文章：

[Chnroutes的使用（Mac OS X Mavericks）](http://markzhang.cn/blog/2013/12/04/chnroutes-on-mac/)

最后附上目前最新版本的jvpn 0.7.0：[jvpn-0.7.0.tar.bz2](/downloads/soft/jvpn-0.7.0.tar.bz2)
