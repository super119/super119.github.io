---
layout: post
title: "在Octopress中使用友言，替换Disqus"
date: 2013-11-05 21:56
comments: true
categories: Blog
---

Disqus应该是国外做的一个评论系统，按现在时髦的话说，不太接中国的地气，所以考虑换一个评论系统。具体做起来非常的简单，Octopress就像一个模板系统，以前Java火热的时候用过Velocity，大体思路都是利用一些markdown的语法填在预先写好的模板文件中，然后解析器解释该文件，从而生成最终需要的东西。

在Octopress中，发表的博客文章使用的模板是`source/_layout/post.html`，打开之后搜索disqus就可以发现原来的disqus部分的代码。所以要将其替换成其他的评论系统就很直观了。这里我使用了友言，因为Google了一下貌似现在国内友言做的还不错。

总体来说就是三步：
<!-- more -->

1. 在`_config.yml`中添加一个配置项，比如`uyan_id: xxx`
2. 添加一个被include的文件，用来存放友言提供给我们的插入代码，比如：`source/_include/post/uyan_comments.html`
3. 修改`source/_layout/post.html`，将disqus的部分替换成使用友言。

以下就是我的修改：

{% include_code 上述三点的改动 diff uyan-comments-changes.diff %}


**Note:** 有一个注意点就是友言会采集当前域名作为评论来源的一个index，来归类存放你的留言。所以在`rake preview`的时候，是看不到友言的评论框的。只有将你的博客上传到了网络上之后，你才能看到。因为很多时候我们开发完了都会使用`rake preview`先在本地`http://localhost:4000`上看一下，所以这是看不到的，并不是代码有问题。
