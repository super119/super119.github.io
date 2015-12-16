---
layout: post
title: "Linux kernel: How to debug mutex deadlock 如何调试mutex死锁"
date: 2015-12-16 15:47
comments: true
categories: linux-kernel
---

和userspace调试lock一样，主要是要：

1. 找到lock死锁的地方，打印出调用栈

2. 找出这个mutex目前被谁占用了

<!-- more -->
 
针对1：在kernel config中，Kernel Hacking这个大项中，enable跟lock/mutex相关的config，比如CONFIG_DEBUG_MUTEX，以及CONFIG_DETECT_HUNG_TASKS, 这样当死锁发生时，稍微等待一段时间（默认120秒），kernel就会检测到死锁，同时打印出调用栈。在kernel hacking里面，包含了大量的有利于调试的config，可以一项一项都看一遍，总有一款适合您。


针对2：来到代码中死锁的地方，在enable了CONFIG_DEBUG_MUTEX的情况下，添加这样的代码：

``` cpp
show_stack(<lock>.owner, NULL);
```

<lock>是你的mutex的变量名，.owner是一个struct task_struct *，利用show_stack函数就可以打印出该mutex被谁占用了，而且占用该mutex时的调用栈。非常cool。

此外可以看一下struct mutex的定义，有一些实用的东西，比如mutex的name。struct task_struct中也有一些实用的东西，比如pid等。

所以总结来说，kernel中目前可以打印调用栈的有：

dump_stack：打印当前的backtrace

show_stack：打印指定task_struct的backtrace 
