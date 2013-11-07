---
layout: post
title: "Linus说，请将这个配置加到你的.gitconfig"
date: 2013-11-07 18:18
comments: true
categories: git
---

近日Linus说，请将 `git config core.abbrev 12` 加到你的git repository中。事实上，这个config很有意义，可以加到你的全局git config中：

<!-- more -->

`git config --global core.abbrev 12`。

原因简单来说就是，git默认的7个字符的短HASH表示方法经常导致冲突，所以Linus建议改成12个字符。原文引用如下：

>Linus jumped in to say that he would like everybody to run this command in their repositories:

>    git config core.abbrev 12

> That causes git to abbreviate commit hashes to 12 characters. The default of seven characters is too small to prevent occasional hash collisions in the kernel; it was, he said, a big mistake made early in git's history. He also noted that he spends a lot of time fixing up hashes in patch, many of which are "clearly bogus." Most of the problems, most likely, are caused by the rebasing of trees.
