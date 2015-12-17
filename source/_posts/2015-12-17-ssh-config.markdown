---
layout: post
title: "SSH client config Sample"
date: 2015-12-17 11:01
comments: true
categories: linux
---

man ssh_config，查看~/.ssh/config的语法。

Host可以使用通配符，当ssh的时候如果server的URL能match上这里Host指定的值，则Host下面指定的HostName将被作为最终URL使用。同时该Host下配置的User, Port都将被使用。

当然，User和Port都可以被命令行上指定的override。 

附上目前的一份sample配置：

<!-- more -->

``` bash
Host chromium
	HostName gerrit.chromium.org
	User markz
	Port 29418
	PubkeyAuthentication yes
	IdentityFile ~/.ssh/chromium
Host bitbucket.*
	HostName bitbucket.org
	PubkeyAuthentication yes
	IdentityFile ~/.ssh/bitbucket
Host gitorious.*
	HostName gitorious.org
	PubkeyAuthentication yes
	IdentityFile ~/.ssh/gitorious
Host gitse
	HostName gitse.nvidia.com
	PubkeyAuthentication yes
	IdentityFile ~/.ssh/gitse
Host github.*
	HostName github.com
	PubkeyAuthentication yes
	IdentityFile ~/.ssh/github
Host *.cyanogenmod.*
	HostName review.cyanogenmod.org
	PubkeyAuthentication yes
	IdentityFile ~/.ssh/cmreview
Host *
	PubkeyAuthentication no
```

在最末尾默认禁止Public key的验证方式，也就是说，优先采用password验证。否则有些ssh server就会登录不上，因为有些ssh server优先验证public key，而我们这里有很多public key，一个一个验证失败之后就会因为验证失败次数太多从而连不上server。 
