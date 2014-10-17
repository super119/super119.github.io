---
layout: post
title: "ChromeOS配置SSH Server"
date: 2014-10-17 12:52
comments: true
categories: linux
---

ChromeOS是一个相当"Linux"的OS。现在一般能拿到手的ChromeOS的设备很多都是Chromebook，所以本文是在一台Chromebook上做的测试。

ChromeOS默认的零售版本是什么开发类工具都没有的，包括ssh client都没有。要enable SSH client/server，首先要将ChromeOS切换到 `Developer Mode`。在我手上的这台Chromebook上，做法很简单，就是三键同按开机即可（ESC + Refresh + Power）。

具体的切换到 `Developer Mode` 的做法可以参考Google的ChromeOS网站上的文章。接下来来看怎么enable SSH Server。

<!-- more -->

以下内容完全参考了文章：

[http://www.de7ec7ed.com/2013/05/ssh-daemon-samsung-chromebook-exynos.html](http://www.de7ec7ed.com/2013/05/ssh-daemon-samsung-chromebook-exynos.html)

所以在这里基本上是做一个英翻中的工作。

- 首先启动Chrome浏览器，按下 `ctrl + alt + t`，开启Crosh（浏览器界面的一个Terminal程序）。Crosh也是只有将ChromeOS切换成 `Developer Mode` 之后才有的。

- 输入：`shell`，得到一个bash，然后输入 `sudo -i`，就可以得到root权限了。

- mkdir -p /mnt/stateful_partition/etc/ssh

- ssh-keygen -t dsa -f /mnt/stateful_partition/etc/ssh/ssh_host_dsa_key

- ssh-keygen -t rsa -f /mnt/stateful_partition/etc/ssh/ssh_host_rsa_key

- vi /etc/ssh/sshd_config，将 `PasswordAuthentication` 改成 `yes`。在这里也可以看到，为什么我们要在/mnt/stateful_partition下面创建etc/ssh目录，并在里面生成keys。就是因为在这个配置文件中指定了ssh key的路径。

- /sbin/iptables -A INPUT -p tcp --dport 22 -j ACCEPT

- 给root用户设置一个密码，输入passwd即可。

- /usr/sbin/sshd，这样SSH server就启动起来了，可以从其他机器登录过来了。

题外话：如果你是 `cd /usr/sbin;./sshd`，或者 `/usr/sbin` 在PATH里面，直接输入 `sshd` 启动，你会发现sshd无法启动，错误是：

> sshd re-exec requires execution with an absolute path

Stackexchange有这个问题的回答，很有意思，有两点：

> This is specific to OpenSSH from version 3.9 onwards.
> 
> For every new connection, sshd will re-execute itself, to ensure that all execute-time randomisations are re-generated for each new connection. In order for sshd to re-execute itself, it needs to know the full path to itself.
> 
> Here's a quote from the release notes for 3.9:
> 
> Make sshd(8) re-execute itself on accepting a new connection. This security measure ensures that all execute-time randomisations are reapplied for each connection rather than once, for the master process' lifetime. This includes mmap and malloc mappings, shared library addressing, shared library mapping order, ProPolice and StackGhost cookies on systems that support such things

另外一个说法也有一定道理：

> This is to prevent someone from injecting a sshd program somewhere in one of the directories in your PATH and you inadvertently executing it.

回到SSH server的配置上来，如果想ChromeOS开机的时候启动SSHD，写这样一个脚本放到 `/etc/init` 目录下面即可：

``` bash
start on started system-services
script
     /sbin/iptables -A INPUT -p tcp --dport 22 -j ACCEPT
     /usr/sbin/sshd
end script
```

最后，突然想到，要做上面的所有的事情，要求ChromeOS的root partition是可写的，但是默认情况下，ChromeOS的root partition是read-only的。需要修改的话这样做：

- /usr/share/vboot/bin/make_dev_ssd.sh --remove_rootfs_verification --partitions 2

- 做完上述命令后，需要重启ChromeOS

- 重启完成之后，再开启一个Crosh，执行：mount -o rw,remount /，就可以将root partition变成可写的了。
