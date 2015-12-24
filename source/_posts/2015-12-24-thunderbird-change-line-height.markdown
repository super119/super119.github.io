---
layout: post
title: "Thunderbird on Ubuntu 12.04 - 调整邮件列表行间距"
date: 2015-12-24 09:14
comments: true
categories: linux
---

很不幸必须手动修改配置文件来做到，Thunderbird本身没有提供这样的配置项：

<!-- more -->

``` bash
markz@markz-hp6200:~$ cd .thunderbird/
markz@markz-hp6200:~/.thunderbird$ ls
a8feo3zj.default  Crash Reports  profiles.ini
markz@markz-hp6200:~/.thunderbird$ cd a8feo3zj.default/
markz@markz-hp6200:~/.thunderbird/a8feo3zj.default$ cd chrome/
markz@markz-hp6200:~/.thunderbird/a8feo3zj.default/chrome$ ls
userChrome.css
markz@markz-hp6200:~/.thunderbird/a8feo3zj.default/chrome$ cat userChrome.css 
treechildren:-moz-tree-row {
height: 35px !important;
}
markz@markz-hp6200:~/.thunderbird/a8feo3zj.default/chrome$
```

这里注意如果没有发现那个 `chrome` 的目录，那就自己创建一个，`userChrome.css` 也自己创建，然后将内容填入即可。全部做完重启Thunderbird就能看到效果了。
