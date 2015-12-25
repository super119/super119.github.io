---
layout: post
title: "Linux通过sendmsg在进程中共享FD"
date: 2015-12-25 10:57
comments: true
categories: linux
---

对于现代Linux来说，FD的使用变的越来越广泛。FD不光用于一个文件或设备的打开句柄，现在FD还往往会用于一些其他数据类型的句柄，比如一块buffer。

对于一些要避免拷贝的buffer来说（比如一块用于display的buffer，或者一块用于render的buffer），使用FD来表示这块buffer并在不同进程中传递就被使用的越来越广泛。而且进程退出（不管是正常退出还是被杀）的时候，FD还能被kernel回收，再加上 `reference count`，这块buffer就不会有泄漏的问题。看看Android和Linux kernel的代码就会发现，现在使用FD的这种例子是越来越多了。

在userspace，进程共享FD可以通过 `sendmsg` 来实现：

<!-- more -->

man 7 unix

man sendmsg

sendmsg可以发送control message，也就是不是普通的regular数据。看sendmsg的manual可以知道。

control message其实也是指定一个指针和一个长度，和普通数据一样。

有趣的事，control message可以发送一个FD（或一组，以数组方式组织）给另外一个进程。不是说把FD这个数字发给了别人，而是这个FD真正的另外一个进程中存在，而且和发送进程的FD指向的是一个struct file

简单来说，就好象接收进程dup了发送进程的那个FD一样。事实上嘛，大家都懂的，其实就是kernel帮助接收进程dup了一下而已。。。

在man 7 unix中搜索SCM_RIGHTS可以看到具体用法和信息。

有个条件是：必须工作在UNIX这个socket domain上（AF_UNIX），TCP/IP socket domain是不行的。 
