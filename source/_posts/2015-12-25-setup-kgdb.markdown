---
layout: post
title: "KGDB配置使用"
date: 2015-12-25 10:49
comments: true
categories: linux-kernel
---

KGDB可以用于linux kernel的调试，具体的配置步骤是：

<!-- more -->

make menuconfig, under "Kernel Hacking", enable "KGDB" & "KGDB over serial line". Enable "compile kernel with debug info"(CONFIG_DEBUG_INFO) & "enable frame pointer"(CONFIG_FRAME_POINTER) options(both are under "Kernel Hacking").

有关CONFIG_FRAME_POINTER这个option，需要在enable了ARCH_CONFIG_FRAME_POINTERS这个option之后才会出现在Kernel hacking下面。

这ARCH_CONFIG_FRAME_POINTERS在make menuconfig里面是找不到的。这个option是专门给其他人select用的。一般都是在ARCH config的时候会去select这个option

比如x86下，一般会enable这个option。而ARM则不会（可能是考虑到生成的kernel文件尺寸和性能）。

所以，在ARM下，可以编辑arch/arm/Kconfig，在CONFIG_ARM下面的一堆select的最后，添加上一句select CONFIG_ARCH_FRAME_POINTERS就可以了。 

从实验结果来看，只要enable了CONFIG_DEBUG_INFO，基本上就差不多了，能看到代码了。 

***

Add kernel option: `kgdboc=ttyS0,38400 kgdbwait` into kernel command line.

根据我的实验，这里115200不行。在后面gdb设置target remote /dev/ttyS0的时候，会被告知最高支持到38400.

所以，这里如果115200不行，就改成38400。 

***

Start kernel, kernel will wait for connection from remote PC's gdb. 

On remote machine, "<gdb path> ./vmlinux" -- this "vmlinux" mostly is under the root directory of kernel(a big kernel image with debug infos, not the one under arch/arm/boot, that is stripped version).

需要注意的是，这里的GDB必须使用host是x86-64，target是ARM的那种GDB。我们PC上的gdb一般host和target都是x86-64。

这样的GDB可以在一些常见的toolchain网站找到，比如linaro。 

***

Enter gdb commands:

``` bash
set remotebaud 38400
target remote /dev/ttyS0
```

***

Done. You can list source codes, set breakpoints now. Continue the debugging after finished.

CAUTION: Close any other programs(normally it's minicom which we use to monitor serial outputs) which may occupy "/dev/ttyS0" before running gdb on remote machine. Otherwise gdb can't connect with target board.  
