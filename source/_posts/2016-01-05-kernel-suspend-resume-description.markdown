---
layout: post
title: "Kernel resume的逻辑"
date: 2016-01-05 15:59
comments: true
categories: linux-kernel
---

Resume的时候，其实最开始是从bootrom启动，再到uboot，然后再到kernel，到kernel的common code的时候，arch相关的代码都已经跑完，此时CPU已经起来了（其实CPU起来之后执行的第一句代码位于设定好的CPU reset handler指定地方的代码，一般这是一段汇编，在这里会去读取suspend结束的时候的一些系统状态，从而可以顺利的转到kernel层继续）。

<!-- more -->

所以到了kernel这边的时候，是从suspend结束时的下一条指令开始继续执行。具体就位于suspend.c的suspend_enter的下半部分代码。

在这里就可以看到，比如dpm_resume_noirq就是关键函数。在这里就会依次去调用所有device对应的driver的resume方法，顺序是先suspend的后被resume，后suspend的先resume。

结合之前说的suspend的逻辑，综合一下就是：

device_add的时候：

先被add的device，后被suspend，先被resume

后被add的device，先被suspend，后被resume

不过这不是绝对的，细节一点来说，这里面还分noirq的device resume list和irq enabled device resume list，具体的可以去看源代码。

然后suspend_enter函数就会被执行完成，按照之前suspend时候的调用栈，就会回到函数：suspend_devices_and_enter这个函数中调用suspend_enter的下一句继续执行，在这里又会做一些resume的工作，比如console resume就在这里。

最后就回到了调用suspend_devices_and_enter的函数enter_state，在这里会调用suspend_finish，这个函数里面就会调用到suspend_thaw_processes，这个函数就会thaw workqueues, thaw tasks，然后调用schedule开始进程调度，到这个时候userspace也就活过来了。
