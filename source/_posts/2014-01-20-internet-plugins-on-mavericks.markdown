---
layout: post
title: MacOS X Mavericks上的浏览器插件的问题
date: 2014-01-20 21:30
comments: true
categories: MacOSX
---

升级了Mavericks之后，发现浏览器中的一些插件出问题了。最先遇到的就是在Safari中经常会蹦出来对话框说，是否要在该网站上运行迅雷（Thunder xxx）插件，有几个选项，yes/no/never for this site.... 后来又发现旺旺不工作了，也就是在浏览器中点击旺旺图标，旺旺出不来了....

所以花了点时间看了一下，原来现在在Safari中有专门的 `Internet plug-ins` 这样的设置了，而且该设置影响安装的所有浏览器（如Chrome, Firefox）。

<!-- more -->

首先进入Safari的Preferences（COMMAND + ,），在 `Security` 标签中，有 `Internet plug-ins` 这一项，我们需要先点击 `Allow Plug-ins`，然后点击后面的 `Manage Website Settings...` 按钮出现对话框：

![Manage Website Settings](/downloads/image/manage-website-settings.png)

在这里就可以设置每个plugin的处理方式了，如上图所示，针对旺旺插件，我们选择了 `Allow Always`。你还可以在这里找到其他很多你需要用到的插件，比如淘宝的 `AliEdit`，也就是密码输入框的插件、迅雷的下载插件等等。值得一提的还有就是支付宝的插件，名字有点怪，叫：`npalicdo plugin`。

有意思的是，在MacOS X上，所有的插件是统一安装在一个地方的：`/Library/Internet Plug-Ins`。所以插件一旦安装了，所有的浏览器就都能使用了。只是每个浏览器是否使能某个插件，在各自的设置里面，如上面介绍的Safari的。在Chrome中，那就是在页面：`chrome://plugins` 里面。

所以其实比如下载了支付宝的浏览器插件，安装的时候，其实就是把文件拷贝到了 `/Library/Internet Plug-Ins` 这个目录下而已。

