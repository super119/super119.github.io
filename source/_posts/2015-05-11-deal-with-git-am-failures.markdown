---
layout: post
title: "git am失败的时候怎么办"
date: 2015-05-11 10:15
comments: true
categories: git
---

本文主要参考了以下文章:

[http://www.pizzhacks.com/bugdrome/2011/10/deal-with-git-am-failures/](http://www.pizzhacks.com/bugdrome/2011/10/deal-with-git-am-failures/)

`git am` 或 `git am -3` 打patch的时候经常会遇到fail，就是因为patch不apply。但是 `git am` 不像 `git cherry-pick` 一样会显示冲突的地方从而让我们可以修改，很多时候 `git am` 并不显示冲突，只是让我们修正冲突，这就产生了麻烦。

<!-- more -->

于是上面的文章就很好的解决了这个问题，因为一般产生冲突的时候，并不是所有的改动都有冲突，往往冲突是比较少的（`patch` 本身是可以智能的自动解决一些冲突的），所以我们就可以先用 `git apply` 来打patch，并将冲突存为一个单独的文件，最后查看冲突文件再手动将冲突修正就可以了。

引用上面文章中的内容，当 `git am` 失败的时候，可以这么做：

``` bash
$ git apply PATCH --reject
$ edit edit edit
$ git add FIXED_FILES
$ git am --resolved
```

当 `git am` 失败的时候，它会告诉你patch文件被它放在了一个路径下面，所有，你只需要直接去 `git apply --reject` 这个patch就好了：

``` bash
The copy of the patch that failed is found in...
        xxxxxx
```
