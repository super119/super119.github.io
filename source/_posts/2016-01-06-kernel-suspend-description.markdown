---
layout: post
title: "Kernel suspend的逻辑"
date: 2016-01-06 09:44
comments: true
categories: linux-kernel
---

代码位于：power/suspend.c，从函数pm_suspend开始。

suspend的逻辑和driver相关的主要就是遍历所有device，并调用他们相关driver的suspend函数指针（包括该device的class，bus这些的suspend函数指针）。

<!-- more -->

至于device调用的先后次序是这样的：当我们执行device_add的时候，这些device会被加到一个dpm_list的list中，而且晚add的device会出现在list的靠前位置。也就是说，早add的device会晚一点被suspend，最后add的device会第一个被suspend。

resume的时候就正好反过来，最后add的device会第一个被resume。

而具体在suspend一个device的时候，会依次调用这些函数指针：device所属的power domain，device的type，device所属的class，device所属的bus。

在上述的逻辑之前，会有一个名为suspend_prepare的调用，在这里会freeze userspace（不schedule了就OK了），会freeze有freezable属性的workqueue。在上述逻辑之后，kernel就会转到arch层继续做suspend的动作。在这里就是每种arch，每种mach各不相同了。在这里一般就是针对具体的硬件，做state saving，clock gate，power gate，prepare cpu reset handler这样的一些动作了。

上面提到的有Freezable属性的这个workqueue比较实用，在suspend的时候会freeze，这样就会避免当suspend的时候，一些设备被关闭（或者clock被gate，或者power被gate），但是workqueue中还有work在运行从而导致一些问题。

跟了一下代码，这个system_freezable_wq主要就是在create workqueue的时候加上了一个FREEZABLE的flag，然后当suspend发生的时候，在没有开始真正suspend device之前，有一个suspend_prepare会调用suspend_freeze_processes，这样一路调用下来，最终会调用到freeze_workqueues_begin，在这里会将workqueue的maxactive设成0，这样workqueue里面的work就无法执行到了。
