---
layout: post
title: "编译out-of-tree的kernel模块"
date: 2015-05-12 10:51
comments: true
categories: linux-kernel
---

所谓 `out-of-tree` 的内核模块，指的是源代码不在kernel tree里面的模块，比如 `nouveau` 的tree。

熟悉nouveau的都知道，nouveau的maintainer是单独维护nouveau tree的，所以，一般来说，我们会把nouveau的代码直接放在一个目录下，然后将nouveau编译成一个模块（.ko），这样来编译运行测试就都比较简单。

<!-- more -->

所以关于如何编译一个 `out-of-tree` 的内核模块，其实内核是有官方文档的：

[https://www.kernel.org/doc/Documentation/kbuild/modules.txt](https://www.kernel.org/doc/Documentation/kbuild/modules.txt)

我一般是这么编译的：

``` bash
make ARCH=arm64 CROSS_COMPILE=<your toolchain> INSTALL_MOD_PATH=<kernel module install path> M=~/nouveau/drm/nouveau/ -C <the kernel root directory> -j4 modules
make ARCH=arm64 CROSS_COMPILE=<your toolchain> INSTALL_MOD_PATH=<kernel module install path> M=~/nouveau/drm/nouveau/ -C <the kernel root directory> modules_install
```

一些注意点：
- 所谓 `kernel root directory` 就是能找到.config文件的那个目录
- 使用 `M=` 来指定nouveau在哪里，注意这里要指到 `KBuild` 所在的那个目录
- kernel本身也有nouveau driver（位与drivers/gpu/drm/nouveau），所以在编译我们的nouveau之前，要先在kernel config中设置编译nouveau为module，然后编译kernel，这样就会出现一个nouveau.ko（来自drivers/gpu/drm/nouveau），这个时候才可以开始用上面的命令行来编译我们自己的nouveau。而我们的nouveau编译完成，module_install的时候，会被安装到 `extra` 目录下，所以不会和原来的nouveau.ko冲突。但是为了避免困扰，我们可以将原来的nouveau.ko删掉。
- 如果你曾经使用 `O=<dir>` 设置了kernel编译的output dir，那么上面的命令行有可能会不work


