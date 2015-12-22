---
layout: post
title: "repo init 中指定manifest和branch的含义"
date: 2015-12-22 14:59
comments: true
categories: git
---

比如：

repo init -u git://xxx/xxx/manifest.git -b <BRANCH> -m <MANIFEST>

<!-- more --> 

这里-m和-b的含义是：

1. 注意到manifest.git本身也是一个git project

2. 所以，-b指定的是使用#1中这个git project的哪个branch

3. -m指定的是，下载该git project中的哪个文件（应该是首先切换了branch，然后再下载该文件）。

repo init这样做过了之后，其实本地就建立起来了一个.repo目录，里面核心文件就是manifest.xml，这个xml中就定义了：

- 包含哪些git project

- 每个git project使用什么branch

- 每个git project在本地磁盘上的路径和远端服务器上该git project的路径。 

事实上，当repo init完成，在本地的.repo/manifests目录下就可以看到manifest这个git project，可以用git branch -av来查看branch，用git log来查看日志。 
