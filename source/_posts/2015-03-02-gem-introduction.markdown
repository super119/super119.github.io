---
layout: post
title: "GEM - Graphics Execution Manager 介绍"
date: 2015-03-02 11:15
comments: true
categories: linux-kernel repost
---

本文来自：[http://lwn.net/Articles/283798/](http://lwn.net/Articles/283798/)

NOTE: Currently the buffer creation/read/write are implemented by driver specified ioctls. So right now you can't find functions like "drm_gem_create" in kernel sources.
And that's also why we need "dumb buffer" in drm's ioctls(dumb_create, dumb_destroy...)

<!-- more -->

原文请看：[http://lwn.net/Articles/283798/](http://lwn.net/Articles/283798/)

GEM是Intel建议提出并实现的，所以这篇文章是GEM maintainer写的introduction，非常详尽，只是对照现在的kernel来说，内容已经有一点outdate（所以我写了上面的NOTE），但是大部分内容都是OK的。
