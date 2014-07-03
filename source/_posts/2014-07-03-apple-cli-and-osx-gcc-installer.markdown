---
layout: post
title: "Mac OS X(Mavericks) CommandLineTools 和 osx-gcc-installer"
date: 2014-07-03 13:22
comments: true
categories: MacOSX
---

Mac OS X Command Line Tools是Mac开发人员经常会遇到的一个问题。Command Line Tools（以下简称CLI）其实就是一堆Unix/Linux上开发需要的基础东西，比如gcc, make, g++等。在Mac上开发，如果使用Xcode的话，是不需要这些东西的，因为Xcode已经集成了这些东西，在Xcode的IDE中直接进行编译调试，所以用户不需要关心这些东西有没有，在哪。

但是如果我们在开发自己的基于Makefile的程序时，或者是要porting UNIX/Linux上的一些程序时，这些东西就变成必须的了。

<!-- more -->

苹果官方的CLI在苹果的网站上可以下载到，地址不需要自己去找，如果你已经安装了Xcode的话，那启动Xcode，选择菜单：`Xcode -> Open Develop Tool -> More Develop Tools`，然后浏览器就会被启动并来到下载苹果CLI的地方。下载dmg然后安装就完了。

苹果的CLI安装完成之后，其实是安装了两个pkg：

``` bash
com.apple.pkg.CLTools_Executables
com.apple.pkg.MacOSX10_9_SDK
```

用 `pkgutils --files <pkg name>` 就可以看到安装了哪些文件，在什么地方。所以CLTools_Executables这个package，是安装在 `/Library/Developer` 目录下的，所以如果要使用gcc的话，把路径：`/Library/Developer/CommandLineTools/usr/bin/` 添加到PATH中去。

你可以看到，苹果的这个CLI，有很多llvm, gcc, clang这样的东西，这里有篇文章解释的很好：

[http://www.cnblogs.com/qoakzmxncb/archive/2013/04/18/3029105.html](http://www.cnblogs.com/qoakzmxncb/archive/2013/04/18/3029105.html)

说来就是这么简单的事情，但是你如果在Google上尝试寻找答案的话，你会发现这些答案基本上在Mac OS X 10.9（Mavericks）上都没法用，典型的答案有：

- 运行 `xcode-select --install`，反正在我的机器上，`xcode-select` 压根没有 `--install` 这个option。
- 执行程序：`/System/Library/CoreServices/Install Command Line Developer Tools.app`，其实这就是 `xcode-select --install` 去执行的程序，在我的Mac上运行没反应。
- 还有一些文章说直接在终端运行gcc，就会跳出一个对话框让你安装CLI的，我也没有碰到这么神奇的事情。

现在再来说说 `osx-gcc-installer`。

`osx-gcc-installer` 是github上一个开源的东西，估计是作者自己编译了苹果的一些open source的东西得到的：

[https://github.com/kennethreitz/osx-gcc-installer](https://github.com/kennethreitz/osx-gcc-installer)

我对这个东西的理解是，在Mac OS X Lion/Mountain Lion之前，要安装苹果的CLI，是需要首先安装Xcode的（貌似是需要在安装了Xcode之后，在Xcode中有一个地方可以下载CLI），所以为了让我们在不安装Xcode的前提下得到gcc/make，就有了这个项目。该项目的README文件印证了这一点：

> For Lion and Mountain Lion users, Apple now provides an official Command Line Tools for Xcode package that you can install without needing to install Xcode itself! You can download it from Apple's developer site (free registration required) and search for "Command Line Tools".

之前我也安装过这个东西，现在如上面所说，我已经安装了苹果官方的CLI，所以很自然的我就想卸载掉这个东西，照例Google了一大圈之后，发现一般都是这么说的：

> According to the creator of the package, you can install Xcode on top of it since it is just a subset. Then you can use the uninstall command to completely remove:
> 
> sudo /Developer/Library/uninstall-devtools --mode=all
> 
> Definitely not a quick uninstall, but it should work.

以上摘自StackOverflow: [http://stackoverflow.com/questions/9332196/how-to-remove-the-osx-gcc-installer](http://stackoverflow.com/questions/9332196/how-to-remove-the-osx-gcc-installer)

但事实上，这个做法在Mavericks上也是不行的，首先，我压根就没有 `/Developer` 这个目录，据说是Xcode从4.3开始就是一个“绿色软件”，所有东西都在 `/Applications/Xcode.app` 下面了。

此外，事实上我之前的确装过老版本的Xcode，在安装了 `osx-gcc-installer` 之后，的确有 `/Developer` 这个目录，但是运行了那个脚本之后，什么事情都没发生。`/usr/bin/gcc, /usr/bin/llvm-gcc-4.2` 这些文件还在那。

所以还是自己想办法了，通过 `pkgutils --expand GCC-10.7-v2.pkg tmp` ，可以在 `GCC-10.7-v2.pkg` 解开（这个pkg也就是osx-gcc-installer在Mac OS X 10.8上的安装文件），可以看到这个pkg包含了9个package：

``` bash
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 DevSDK.pkg
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 DeveloperToolsCLI.pkg
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 DeveloperToolsSystemSupport.pkg
-rw-r--r--  1 mark  staff   6.7K  8 12  2011 Distribution
drwxr-xr-x  3 mark  staff   102B  7  3 14:00 Resources
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 VersionedDeveloperToolsSystemSupport.pkg
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 X11SDK.pkg
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 XcodeUserSystemSupport.pkg
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 clang.pkg
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 gcc4.2.pkg
drwxr-xr-x  6 mark  staff   204B  7  3 14:00 llvm-gcc4.2.pkg
```

所以我使用 UninstallPKG 这个软件将这9个packag都删除掉就OK了。UninstallPKG是收费软件，售价9.99美元，试用版只能执行三次Uninstall，不过通过使用我发现不是只能uninstall 3个package，而是限制只能使用3次，也就是说，你可以一次选择多个package一并删除。

所以打开UninstallPKG，选择这9个package然后uninstall即可。注意这9个package的包名是上述名字后面都加了一个 `Leo`（估计是Snow Leopad的意思）。

![UninstallPKG](/downloads/image/uninstallpkg.png)

所以到此简单来说，现在由于苹果提供了官方的CLI，所以其实 `osx-gcc-installer` 这个东西已经不再需要了。除非有一些你需要的其他东西依赖它。貌似现在新版的 HomeBrew 也是需要苹果的CLI而不是osx-gcc-installer了。`osx-gcc-installer` 的TroubleShooting是这样说的：

> Do not install this if you already have Xcode installed! Mixing Xcode and osx-gcc-installer is known to cause various difficult-to-diagnose problems and is not recommended.
