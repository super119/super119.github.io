---
layout: post
title: "在Mavericks上使用Makefile编写wxWidgets程序"
date: 2014-04-23 23:31
comments: true
categories: OpenGL
---

OpenGL的设计与操作系统无关，与窗口系统也无关，所以，要编写一个可以运行（能看到界面）的程序，还需要一些额外的GUI的框架，比如glut。

glut基本上是所有OpenGL教科书上的唯一推荐了，因为它的API简单，跨平台。所以读者不需要花额外的时间去了解并使用glut。但是glut比较老了，有点跟不上时代了。而且编译出来的东西是一个可执行文件，不是Mac OS X上的 `.app` 的那种Applicaiton。所以在StackOverflow上也有很多人推荐使用 `glfw`。我也去试用了一下，感觉还是太单薄了。

既然要编写一个像样的OpenGL程序，就选择一个像样的GUI framework library吧，于是我决定使用wxWidgets。

<!-- more -->

## wxWidgets的编译安装 ##

wxWidgets在Mac OS X Mavericks上的安装很简单，就是标准的configure/make/make install。只是configure需要指定一些option，否则wxWidgets无法找到一些必须的库。

结合了wxWidgets wiki上的一些内容，我的编译过程是这样的：

``` bash
cd <root directory of wxWidgets source>
mkdir build-release
cd build-release
../configure --with-osx_cocoa --with-macosx-version-min=10.7 --with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk --prefix="$(pwd)" --with-opengl
make
```

我安装的还是较老版本的Xcode，所以可以看到上面的命令行中，SDK用的还是10.8的，不过不妨碍了。

此外我没有执行make install，因为我也没打算给其他用户来使用。反正make过了之后，该有的东西（wx-config、wxWidgets的libraries）都有了。

还有一点：执行完configure之后，你会发现这样的一行：

``` bash
OpenGL support:   no
```

但其实我们已经加了 `--with-opengl` 了，难道是configure没有找到Xcode中的OpenGL framework？经过一番Google，以及最后使用 `wx-config --libs` 来确认，其实OpenGL的支持已经有了，这个信息是一个错误信息。所以这应该算是一个bug了。

## 第一个使用Makefile的程序 ##

接下来就是重头戏了，因为我不想使用Xcode来编写运行程序，太笨重了，而且写一个HelloWorld也需要在Xcode中建立一个Project，并且设置一堆东西。更何况使用Xcode，也无法详细了解整个编译过程。Linux程序员都懂的。

所以我想要的是一个Makefile，我执行make就可以编译，而且要生成Mac OS X上的applicaiton（.app），而不是像使用glut的时候，只是生成了一个可执行文件。执行这个文件是能看到一个界面，但是无法拖动，某些功能还不正常。

我看到wxWidgets自带的samples，就是可以通过make生成Mac OS X的 `.app` 应用的，所以这一定是可行的。于是Google了一下，果然发现wxWidgets的论坛上有答案：

[http://forums.wxwidgets.org/viewtopic.php?f=23&t=3768](http://forums.wxwidgets.org/viewtopic.php?f=23&t=3768)

这个帖子中，帖子的作者Mark最后就给出了一个Makefile的template，使用这个Makefile就可以编译并最终生成一个 `.app`。

于是立刻把这个Makefile拿过来试验了一下，需要做两处小改动：

- OUTPUTPATH必须要设置，而不是像注释中说的那样可以留空。否则后续的 `mkdir -p` 这个命令会失败。

- 这个Makefile中所有的 `@echo` 命令要换成 `@/bin/echo`。因为这个Makefile中有三行这样的代码：`@echo -n xxxx`，`-n` 的意思是输出末尾不要加换行，但是Mavericks上，这个bash built-in的echo是没有 `-n` 这个option的，所以会导致最终生成的 `PkgInfo` 和 `Info.plist` 中有 `-n` 这样的字符串，从而导致最终的 `.app` 没法运行。在 `man echo` 中也提到了这个事情：

> Some shells may provide a builtin echo command which is similar or identical to this utility. Most notably, the builtin echo in sh(1) does not accept the -n option. Consult the builtin(1) manual page.

做完这两处小改动之后，输入make就可以生成 `.app` 了，Cool！

最后附上我的Makefile的模板：

{% include_code Makefile模板 bash wxwidgets-makefile-template %}

![wxWidgets HelloWorld](/downloads/image/hellowxgl.png)
