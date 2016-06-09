---
layout: post
title: "安装Ubuntu/Windows双系统后恢复grub"
date: 2016-06-09 19:38
comments: true
categories: linux
---

重装系统这个事情发现每两年就要搞一次，windows经常莫名其妙的就起不来了。。。

而对于安装了双系统的机器来说，重装了windows之后如何恢复grub重新实现双启动就是一个问题。网上的方法很多，但是一直没有总结一套能完全解决问题的步骤。所以这次在重装完windows之后将步骤记录如下。

<!-- more -->

这次我是删掉了原来安装的windows 7，换成了windows 10。话说现在windows 10可以在微软的官方网站下载的到，激活码上某宝花个几十块钱也就轻松搞定了。毫无意外的，windows 10安装完成之后，机器启动就看不到grub了，只能启动到windows。接下来就是恢复grub的步骤了。

- 首先需要一个恢复系统，或者说Live ISO，或者说rescue system，都是一个意思。以前我都是用Ubuntu的安装ISO，这次Google了一下，发现有一个boot repair的系统也不错，是开源项目，在这里：

[https://sourceforge.net/projects/boot-repair-cd/](https://sourceforge.net/projects/boot-repair-cd/)

下载这个ISO，然后烧录到U盘上从U盘启动即可。Linux下烧录U盘可以用：`Universal USB installer`，windows下可以用：`UltraISO`。`Universal USB installer` 有针对boot repair的支持，烧录的时候选择boot repair即可。

- U盘启动之后，就能看到一个图形界面（U盘启动会看到一个普通启动项和一个EFI启动项，如果普通启动项不工作那就选择EFI启动），在这里能看到boot repair提供了一个一键修复的按钮，但是这个一键修复对于我的机器不行，有Error报错：`GPT detected. Please create a BIOS-Boot partition......`，Google了一下貌似要解决还挺麻烦，所以决定还是手工来做：

``` bash
sudo umount /mnt/boot-sav/sd*   # boot-repair会将机器上所有它发现的分区都自动mount起来，所以我们先全部umount，然后我们自己操作
sudo mount /dev/sda2 /mnt  # mount Ubuntu root分区，可以用boot repair自带的GParted工具来查看具体是哪个分区
for i in /sys /proc /run /dev; do sudo mount --bind "$i" "/mnt$i"; done
sudo chroot /mnt
update-grub
grub-install /dev/sdb  # 按照自己机器的配置，看把grub安装到哪个硬盘上
update-grub
grub-mkconfig -o /boot/grub/grub.cfg
exit
sudo reboot
```

这样重启就OK了，注意这样做了之后默认是不会出现grub界面的，第一次要在BIOS启动完毕之后，按住 `左shift` ，然后就能看到grub界面了，做过一次之后以后每次启动都会自动出现grub界面，等待读秒。

还有就是上面有一句：`grub-mkconfig`，这是一定要做的，否则你在grub的界面中会看不到windows的启动项。`grub-mkconfig` 会自动分析机器中所有的分区然后给出一个完整的启动列表，非常的好用。

如果担心 `grub-mkconfig` 会覆盖之前的 `/boot/grub/grub.cfg`，可以将之前的 `grub.cfg` 先备份然后再操作。
