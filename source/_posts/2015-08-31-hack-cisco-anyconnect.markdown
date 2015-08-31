---
layout: post
title: "How to HACK CISCO anyconnect secure mobile client"
date: 2015-08-31 08:31
comments: true
categories: network
---

本文参考了以下两篇文章：

[http://superuser.com/questions/284709/how-to-allow-local-lan-access-while-connected-to-cisco-vpn](http://superuser.com/questions/284709/how-to-allow-local-lan-access-while-connected-to-cisco-vpn)

[https://github.com/jbalint/dnsmasq/blob/master/run.sh](https://github.com/jbalint/dnsmasq/blob/master/run.sh)

<!-- more -->

CISCO anyconnect secure mobile client（以下简称anyconnect）是CISCO的一个VPN客户端，很多公司都在采用。但是如果你尝试在一台机器上运行anyconnect，其他机器想通过这台机器也能访问公司内网的话，你就发现这是不行的。这是因为anyconnect做了三件令人恶心的事情：

## 监控路由表 ##

Anyconnect客户端启动并连接VPN成功之后，是会监控系统的路由表的。此时，你可以添加路由（但是不好意思这不管用，因为Anyconnect已经添加了一条默认路由），但是无法删除路由。你可以运行 `route del default`，命令也不会出错，但是之后你用 `route -n` 看一下，发现没有效果 -- 这是因为Anyconnect把路由又添加回来了。

事实上，是Anyconnect的 `vpnagentd` 这个守护进程在监控系统路由表。为此，网上有这么一个解决方案，我测试了一下是OK的：

编写这样一个程序：

``` cpp hack.c
#include <sys/socket.h>
#include <linux/netlink.h>

int _ZN27CInterfaceRouteMonitorLinux20routeCallbackHandlerEv()
{
    int fd=50;          // max fd to try
    char buf[8192];
    struct sockaddr_nl sa;
    socklen_t len = sizeof(sa);

    while (fd) {
       if (!getsockname(fd, (struct sockaddr *)&sa, &len)) {
          if (sa.nl_family == AF_NETLINK) {
             ssize_t n = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);
          }
       }
       fd--;
    }
    return 0;
}
```

然后编译：`gcc -o libhack.so -shared -fPIC hack.c`，最后利用 `LD_PRELOAD` 将其装载：

``` bash
LD_PRELOAD=/opt/cisco/anyconnect/lib/libhack.so /opt/cisco/anyconnect/bin/vpnagentd
```

你可以通过修改 `/etc/init.d/vpnagentd` 这个脚本，添加上面的 `LD_PRELOAD` 部分。

所以原理其实很简单，就是要修改anyconnect监控路由表的那个函数。在上面的代码中，这个函数是：`_ZN27CInterfaceRouteMonitorLinux20routeCallbackHandlerEv`，在anyconnect 3.1版本测试是OK的。

那如果找出这个函数呢？毕竟anyconnect还有MACOS版，windows版等各种不同版本。对于这个，原作者也有一个办法：

``` bash
sudo gdb --pid <vpnagentd pid>
break socket
continue
bt
```

也就是说attach上 `vpnagentd` 进程，然后在 `socket` 这个函数上设置断点，当你设置了断点之后，输入 `continue` 让 `vpnagentd` 继续运行，然后你就可以在命令行下去执行：`sudo route del default`，这样 `vpnagentd` 就会撞上我们设置的断点了，此时用bt就可以看到调用栈了。

得到调用栈之后，你能看到的是类似这样的东西：`CInterfaceRouteMonitorLinux::routeCallbackHandle`，这并不是真正的symbol，这是GDB帮我们解析过的函数名，所以，可以用 `nm`, `objdump` 这样的工具去dump这个函数所在的文件（在我的anyconnect 3.1上，这个文件是：`/opt/cisco/anyconnect/lib/libvpnagentutilities.so`），在dump出来的symbol中去grep这个函数，就可以得到真正的symbol名字了。

如果 `nm` / `objdump` 无法显示出symbol的话，那就是这个库或者可执行文件是strip过的，那这个时候我们就只能用 `hexedit` 这样的工具打开二进制文件，在其中搜索GDB中我们得到的函数名字了。

## 监控 /etc/resolv.conf ##

这是为了防止我们修改DNS，对于此，解决方案要简单一些，毕竟这是针对文件的修改，而Linux kernel有一些对 `inotify` 的设置，所以可以这样做：

``` bash
# go to great lengths to rewrite resolv.conf
# the vpnagentd watches it with inotify and rewrites it immediately after I change it
# set inotify limit to 0, first rewrite, vpnagentd will fix it, but then wont be able to set the watch again....
INOTIFYLIMIT=`cat /proc/sys/fs/inotify/max_user_watches`
sudo bash -c "echo 0 > /proc/sys/fs/inotify/max_user_watches"
sudo bash -c "echo nameserver 127.0.0.1 > /etc/resolv.conf"
sleep 1
sudo bash -c "echo nameserver 127.0.0.1 > /etc/resolv.conf"
sudo bash -c "echo $INOTIFYLIMIT > /proc/sys/fs/inotify/max_user_watches"
```

这个方案我没有测试过，所以我不敢保证一定管用。列在这里只是想说明，对于一个进程监控某个文件这种事情，可以利用kernel的 `inotify` 的一些设置，从而来绕过进程的这种行为。

## 设置iptables ##

Anyconnect是有设置iptables rules的，运行：`sudo iptables -vnL`，就可以看到anyconnect设置的所有rules。

这些rules中，有一个很讨厌的地方就是，它会约束anyconnect只服务于来自本地的请求，也就是说，如果局域网内其他机器想通过拨了anyconnect的机器共享访问VPN资源的话，这是不行的。

解决方法倒是异常简单：`sudo iptables -F` 搞定，貌似anyconnect并没有监控iptables的修改。

## 最后 ##

虽然anyconnect有上述三条类似木马病毒的行为，但是实事求是的说，它本身工作的还是很好的，而且它有适应不同操作系统的各种版本也很不错。

回到开头的话题，在我们搞定了上述三条之后，如果想让其他机器通过拨了anyconnect的机器共享访问VPN资源的话，只需要在拨了anyconnect的机器上再运行两条命令即可：

``` bash
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -o cscotun0 -j MASQUERADE
```

你可以将上面两条命令添加到：`/etc/rc.local` 即可在开机时自动执行了。
