---
layout: post
title: "CloneZilla磁盘克隆，超快的更换硬盘"
date: 2014-06-03 12:49
comments: true
categories: linux
---

开发用的PC机用的是一块128G的SSD，安装的Ubuntu 12.04，速度明显比机械硬盘快了好多，唯一的不足就是容量太小。于是最近购入了一块新的240G的SSD，需要将原来128G的SSD上的东西全部克隆过来。

对于Linux ext4文件分区系统的磁盘来说，`ghost`工作的并不好，工作的比较好的是`CloneZilla`，因为它能识别ext4的文件系统，从而使克隆变的无比快速，之前我写过一篇类似的文章：

[http://www.cnblogs.com/super119/archive/2012/09/26/2703479.html](http://www.cnblogs.com/super119/archive/2012/09/26/2703479.html)

但是那一次的克隆是两块容量一样的磁盘，而这一次两块SSD的容量并不相同，所以我做了一些改变。

<!-- more -->

两块磁盘的容量不一样，我能想到的问题是，如果是克隆整块硬盘的话，那MBR部分也会被克隆过去，也就是说，源磁盘的分区信息也会被克隆到目的盘上，这样，会不会240G的SSD就会变成120G？我没有试验，有可能 `CloneZilla` 能帮我们解决这个问题，但是为了避免麻烦，我还是手动的克隆了。

所以我的做法是，不是克隆整个硬盘，而是只克隆源磁盘上的分区。完成之后，在目的盘上重建MBR信息，也就是在目的盘上恢复grub。具体我是这么做的：

- 使用Ubuntu 12.04安装盘，进入Ubuntu Live（不是安装Ubuntu那个选项），然后使用Ubuntu的Disk Utility给目的盘分区

- 使用CloneZilla的启动ISO启动系统，可以把这个ISO刻成光盘，也可以使用UltraISO这种工具将ISO烧录到U盘上，然后从USB启动电脑。启动起来之后，按照 `CloneZilla` 提供的向导，一步一步进行即可，超级简单。将源磁盘每个分区都克隆到目的盘对应的分区上即可。100G的数据五分钟搞定。

![CloneZilla Cloning](/downloads/image/clonezilla.jpg)

- 接下来就可以将源磁盘从机器中拿出来了，我们只需要再把目的磁盘的MBR恢复就可以成功启动系统了。其实就是恢复grub的标准做法：首先还是使用Ubuntu的安装盘进入Ubuntu Live，然后打开终端，输入以下命令：

``` bash
# Mount root partition to /mnt
sudo mount /dev/sda1 /mnt
sudo mount --bind /dev /mnt/dev
sudo mount --bind /dev/pts /mnt/dev/pts
sudo mount --bind /proc /mnt/proc
sudo mount --bind /sys /mnt/sys

# Chroot into it and update grub
sudo chroot /mnt
grub-install /dev/sda
grub-install --recheck /dev/sda
update-grub
exit

# Quit chroot environment and umount all stuffs
sudo umount /mnt/dev &&
sudo umount /mnt/dev/pts &&
sudo umount /mnt/proc &&
sudo umount /mnt/sys &&
sudo umount /mnt
```

这样就完成了，在我的机器上，/dev/sda就是我新更换的240G的SSD，你需要根据你的机器上的磁盘配置来决定使用哪个磁盘。

上述操作来自文章：

[http://howtoubuntu.org/how-to-repair-restore-reinstall-grub-2-with-a-ubuntu-live-cd](http://howtoubuntu.org/how-to-repair-restore-reinstall-grub-2-with-a-ubuntu-live-cd)

