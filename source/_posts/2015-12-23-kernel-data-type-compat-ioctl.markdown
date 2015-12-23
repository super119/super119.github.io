---
layout: post
title: "Linux kernel data types, alignment, compat_ioctl 数据类型，对齐问题和compat_ioctl"
date: 2015-12-23 10:21
comments: true
categories: linux-kernel
---

特别是在写IOCTL的时候，由于有32bit userspace + 64bit kernel的问题，而且由于architecture的不同，不同data type的length也不同，所以在IOCTL中建议用u32, u64, s32这样无视architecture fix size的data type。

这里是来自LDD的不同的architecture的data type的size：

<!-- more -->

![kernel architecture indepedent data types](/downloads/image/kernel-u32-s32.png)

此外有关IOCTL中structure的alignment的问题，注意添加padding使数据长度alignment，现在一般来说都是添加padding到64bit对齐。这里是为什么需要这么做（针对ARM，x86更不用说，因为x86-64早就有了）：

> Q: I'm new in kernel development. Could you tell me or give me some
> materials to read that why we need to align the size of IOCTL structures
> to 64bit? I can understand if we're working in a 64bit kernel but why we
> need to do this if we're in a 32bit arm kernel? Besides, why the
> pointers in IOCTL structure should be declared as u64?
> 
> A: Because in a few years/months you'll have arm64, but still the same
> driver with the same ioctls ... and if the ioctls are not _exactly_
> the same you get to write compat ioctl code which copies the old 32bit
> struct into the 64bit struct the kernel understands. Hence your ioctl
> must be laid out exactly the same for both 32bit and 64bit, which
> happens if you naturally align/pad everything to 64bits and only use
> fixed-sized integers and no pointers.
> 
> Mark: Ah, I see. Thanks. Yes, u64 still works as 32 bit pointer.

参看LDD十一章中有关Nature Alignment的描述。Alignment主要是牵涉到性能问题，不对齐的数据在fetch的时候会有exception，从而降低performance。

这里是网上搜到的一些建议：

There are some rules that should be followed regardless:

- ioctl commands should never be written in an architecture specific
way. In the example of the OMAP driver, you definitely want to be
able ot use the same command when running Linux on the C6x DSP.

- If possible, use only scalar values as ioctl arguments

- Avoid types that are register sized: 'long', 'size_t', pointer.
Instead use only __u8, __u16, __u32 and __u64 and their signed
versions.

- If you use structures, try very hard to avoid pointers in them,
it messes up all sorts of tools.

- If you use structures, make all members naturally aligned, and pad
the size of the structures to a multiple of the maximum member size.

- Never put sub-command numbers into a structure.

有关compat_ioctl：支持64bit的driver必须要实现的ioctl，当有32bit的userspace application call 64bit kernel的IOCTL的时候，这个callback会被调用到。

Q: Suppose I have defined the following.

``` cpp
#define MY_IOCTL_CMD1 _IOR(MAGIC_NUMBER, 0x01, arg1)
#define MY_IOCTL_CMD2 _IOW(MAGIC_NUMBER, 0x02, arg2)
#ifdef CONFIG_COMPAT
#define MY_COMPAT_IOCTL_CMD1 _IOR(MAGIC_NUMBER, 0x01, compat_arg1)
#define MY_COMPAT_IOCTL_CMD2 _IOW(MAGIC_NUMBER, 0x02, compat_arg2)
#endif
```

Now when we do ioctl from user space, we usually do

ioctl(fd, MY_IOCTL_CMD1, &arg1)

So do we really need to have an ioctl with MY_COMPAT_IOCTL_CMD1 as request?

In the devide code I have handlers defined as follows. ioctl: device_ioctl

``` cpp
#ifdef CONFIG_COMPAT
compat_ioctl: device_compat_ioctl
#endif
```

Can anybody please provide some explanations around this?

--------

A: This compat stuff is for running a 32-bit program in a 64-bit kernel. When you call the ioctl(fd, MY_IOCTL_CMD1, &arg1) from a 32-bit program on a 64-bit kernel, the kernel will divert the ioctl to the .compat_ioctl function in the file_operations struct. This compat_ioctl function is responsible for copying the user argument arg1 as if it were compat_arg1, which uses the 32-bit layout. The compat_arg1 typedef is defined in the kernel so that when compiled for 64-bit, the structure is exactly the same layout as the arg1 compiled for 32-bit.

The definition of MY_IOCTL_CMD1 will take the sizeof arg1 into account when creating the cmd id. When you compile a program for a 32-bit machine, the value for MY_IOCTL_CMD1 will be different than if you compiled it for a 64-bit machine. The 32-bit MY_IOCTL_CMD1 should have the same value as the 64-bit MY_COMPAT_IOCTL_CMD1 in the kernel, however.

There's never a need to use compat_arg1 or MY_COMPAT_IOCTL_CMD1 in a user-space application. Those are only for code compiled in the kernel.
