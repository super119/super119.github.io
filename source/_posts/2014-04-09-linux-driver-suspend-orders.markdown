---
layout: post
title: "Linux设备驱动suspend/resume的顺序"
date: 2014-04-09 16:07
comments: true
categories: linux-kernel
---

Linux在进入suspend的时候，会首先freeze userspace，然后挨个去调用设备驱动程序的suspend，最后调用architecture dependent的callback（非常奔放的描述了，其实有大量的细节问题存在）。Resume就是反过来执行一遍，最后重新enable userspace。那么作为设备驱动程序的编写者来说，主要就是要实现本驱动的suspend/resume逻辑。但是事实上，很多设备之间是有依赖性的，比如在ARM上，很多设备在resume之前，要求 `IOMMU` 要已经被enable，否则硬件上就会出现问题，因为访问的地址不对了。

那么Linux是如何设计/定义各个设备驱动之间的suspend/resume的顺序的呢？

<!-- more -->

简单来说，就是以下几条逻辑：

- 各设备驱动程序按照其对应的设备（device）注册的先后次序，来决定其suspend/resume的顺序。注意是该驱动对应的设备的注册顺序，不是驱动注册的顺序，所以和驱动的注册优先级无关（也就是和module_init, rootfs_init, subsys_initcall这些无关）。越早被注册的设备，其对应的驱动的suspend越晚被调用，其对应驱动的resume越早被调用；越晚被注册的设备，其对应的驱动的suspend越早被调用，其对应的驱动的resume越晚被调用。

- 上面所说的逻辑往往不能满足我们的要求，因为现在的设备（device）都通过DT（Device Tree）的方式进行注册。那么，在DTS文件中，出现的越早的设备就越早被注册（可以参考函数 `of_platform_populate`）。而按照现在upstream linux kernel的约定，设备在DTS文件中，是按照其寄存器的地址，从低到高排序的，所以自然不可能满足我们的需求。

那要怎么办？答案是驱动程序执行probe的时候返回 `-EPROBE_DEFER` 。

对于现在ARM SoC上的设备和其驱动程序来说，Linux kernel都使用Platform device/Platform driver来描述他们。而Platform driver的probe函数就相当于是驱动的初始化函数，一般用来映射寄存器，分配必需的内存等等，看一个驱动程序的代码，一般也可以从Probe开始看起。

Probe函数返回 `-EPROBE_DEFER`，指的是当一个设备驱动程序检测到其依赖的设备的驱动程序还没有ready的时候，返回这个出错码告诉kernel driver framework，我需要延缓自身的probe。还是举上面那个IOMMU的例子，如果一个设备依赖IOMMU，那么该设备的驱动在probe的时候，就应该检测iommu的驱动是否已经ready，如果没有那就要defer自己的probe。在Linux kernel driver framework中，收到驱动probe返回 `-EPROBE_DEFER`，就会将该驱动放入一个list中，一个workqueue来负责遍历这个list，从而再次执行这些驱动的probe函数（事实上，由于kernel driver framework不知道驱动互相之间的依赖关系，所以一旦有一个驱动成功probe了，kernel都会触发这个workqueue，让其去遍历一遍list）。

上面说的是 `-EPROBE_DEFER` 的背景故事了，而事实上，当一个驱动的probe返回过 `-EPROBE_DEFER` ，而最终成功probe之后，kernel就会修改该驱动的suspend/resume的顺序了。这就是为什么驱动probe的时候返回 `-EPROBE_DEFER` 能影响其suspend/resume的次序的原因了：

``` c
/*
 * Force the device to the end of the dpm_list since
 * the PM code assumes that the order we add things to
 * the list is a good order for suspend but deferred
 * probe makes that very unsafe.
 */
device_pm_lock();
device_pm_move_last(dev);
device_pm_unlock();
```

上面的代码来自函数：`deferred_probe_work_func`，也就是上面所说的defer probe的workqueue的工作函数。所以这里可以看到，当一个驱动最终在probe defer的work函数中probe成功之后，kernel会修改该驱动对应的设备（device）在 `dpm_list` 中的位置，而 `dpm_list` 正是kernel suspend/resume 驱动时候遍历的链表。看到调用了函数：`device_pm_move_last(dev);` 没？这个设备被移到了 `dpm_list` 的末尾。

- 事情到此为止，可以说已经有了解决方案：我们可以使用 DEFER PROBE 的方式来调整驱动 suspend/resume 的顺序。不过这里要提一下，还有一条路也是可行的，那就是利用 `syscore suspend/resume`。

syscore是Linux kernel定义的一个framework，我们可以将我们的驱动注册到syscore中去。syscore比较关键的就是 `syscore_ops` 这个结构：

``` c
struct syscore_ops {
	struct list_head node;
	int (*suspend)(void);
	void (*resume)(void);
	void (*shutdown)(void);
};
```

可以看到只有 suspend/resume/shutdown 这三个ops，所以看到这里大概也可以猜到了，注册到syscore中的驱动，其suspend函数是在所有其他驱动的suspend都调用过之后调用的，而其resume函数是在所有其他驱动的resume调用之前调用的。简单来说，syscore的驱动，suspend被晚调用保证其他驱动suspend的时候，syscore的驱动还活着；syscore的驱动，resume被很早调用，从而保证其他驱动resume的时候，syscore的驱动已经活着了。好绕。。。

要将驱动注册到syscore中很简单，调用函数：`register_syscore_ops(&your_driver_syscore_ops);` 就可以了。至于上面说到的syscore的suspend/resume的顺序，实现代码在这里：

``` c
error = syscore_suspend();
if (!error) {
	*wakeup = pm_wakeup_pending();
	if (!(suspend_test(TEST_CORE) || *wakeup)) {
		error = suspend_ops->enter(state);
		events_check_enabled = false;
	}
	syscore_resume();
}
```

以上代码来自函数：`suspend_enter`。可以看到当syscore_suspend被调用之后，就调用architecture dependent的 `suspend_ops->enter` 了，到这里整个系统就已经停下来了，进入suspend状态了。而当resume开始，`suspend_ops->enter` 返回之后，第一个被调用的就是 `syscore_resume`，所以上面那一段听起来好绕的逻辑就这么区区几行代码解释了。

