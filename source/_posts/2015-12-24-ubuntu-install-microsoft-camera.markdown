---
layout: post
title: "Ubuntu 12.04安装Microsoft lifecam studio摄像头"
date: 2015-12-24 09:20
comments: true
categories: linux
---

这是一个UVC摄像头，所以可以使用uvcvideo这个kernel驱动，一般kernel都编译了这个module。

什么是UVC？

UVC，全称为：USB video class 或USB video device class

<!-- more -->

UVC是Microsoft与另外几家设备厂商联合推出的为USB视频捕获设备定义的协议标准，目前已成为USB org标准之一。

如今的主流操作系统(如Windows XP SP2 and later, Linux 2.4.6 and later, MacOS 10.5 and later)都已提供UVC设备驱动，因此符合UVC规格的硬件设备在不需要安装任何的驱动程序下即可在主机中正常使用。使用UVC技术的包括摄像头、数码相机、类比影像转换器、电视棒及静态影像相机等设备。

最新的UVC版本为UVC 1.1，由USB Implementers Forum定义包括基本协议及负载格式。

网络摄像头是第一个支持UVC而且也是数量最多的UVC设备，目前，操作系统只要是 Windows XP SP2 之后的版本都可以支持 UVC，当然 Vista 就更不用说了。Linux系统自2.4以后的内核都支持了大量的设备驱动，其中支持UVC设备。

使用 UVC 的好处 USB 在 Video这块也成为一项标准了之后，硬件在各个程序之间彼此运行会更加顺利，而且也省略了驱动程序安装这一环节。 

所以，apt-get一下guvcview就可以看到图像了。

问题：

** 第一个问题 **

也是uvc驱动官方网站给出的问题，就是有关这款摄像头每次都请求最大USB带宽的问题，需要给uvcvideo这个kernel module添加一个参数，具体做法是：

``` bash
sudo rmmod uvcvideo
sudo modprobe uvcvideo quirks=0x80
重新插拔摄像头
cat /sys/module/uvcvideo/parameters/quirks来检验是否设置成功了。
```

参考资料：http://www.ideasonboard.org/uvc/faq/#faq6

** 第二个问题 **

有的时候会发现好好的图像定格了，或者是整个摄像头设备都没了（在lsusb中看不到了）。此外还有个现象就是，摄像头上的蓝灯会莫名其妙的闪烁起来。Google下，这是由于Chrome中的Peperflash造成的。所以，禁用PeperFlash，安装Adobe的flash即可。做法：

打开chrome，输入about:plugins，点右上角Details，Disable PeperFlash

在Adobe网站下载11.2的flashplayer，可以下载.tar.gz包那个版本，解开，得到libflashplayer.so和一个usr目录。

``` bash
sudo mkdir /opt/google/chrome/plugins
sudo cp <libflashplayer.so> /opt/google/chrome/plugins
sudo cp usr/* /usr
```

重启Chrome，再次进入about:plugins，发现11.2的Adobe flash是enable的，PeperFlash是disable的，就OK了。

貌似PeperFlash是Chrome自带的一个flashplayer，有GPU加速。

Adobe的Linux flashplayer不会再有新版本，但是会有安全更新。 
