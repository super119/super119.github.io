---
layout: post
title: "Octopress - 有条件的为文章添加转载声明"
date: 2013-12-27 15:33
comments: true
categories: Octopress
---

对于原创的文章，我在Octopress的post模板文件中，加入了一段转载声明。但对于一些好的文章，经常也会忍不住想要转载。所以，对于转载的文章，还出现这么一个转载声明就是打自己耳光了。很显然，要做个小手脚，对于转载的文章，去掉这个转载声明。有意思的是，在这个过程中，我居然发现 `liquid` 语法中没有 `逻辑非` ?!

<!-- more -->

模板文件的改动非常简单：

{% raw %}
``` diff
diff --git a/source/_layouts/post.html b/source/_layouts/post.html
index 5d83b4d201f6..12d6466734ec 100644
--- a/source/_layouts/post.html
+++ b/source/_layouts/post.html
@@ -7,11 +7,14 @@ single: true
 <article class="hentry" role="article">
   {% include article.html %}
   <footer>
+    {% if page.categories contains "reprints" %}
+    {% else %}
     <section style="margin-top:15px;background:none repeat scroll 0pt 0pt rgb(220,220,220);padding:0.4em 0.8em">
       <strong>版权所有 &copy;{{ site.time | date: '%Y' }} 转载本站文章请注明： </strong>转载自 http://markzhang.cn<br>
 原文链接: http://www.markzhang.cn{{ page.url }}<br>
 您可以随意地转载本站的文章，但是必须在醒目位置注明来源及本站链接，不可以将本站文章商业化使用，或者修改、转换或者以本作品为基础进行创作。
     </section>
+    {% endif %}
 
     <p class="meta">
       {% include post/author.html %}
```
{% endraw %}

所以，大体上是这样：

1. 代码检查当前的文章是否属于 `reprints` 这个category，如果是的话，就不会加入转载声明那一段文字。
2. 由于Octopress中一篇文章可以属于多个category，所以，新增 `reprints` 这么一个category不会带来什么影响，反而还可以让我们通过这个category快速的浏览所有我们转载的文章。
3. 有意思的就是，正如前面所说，`liquid` 语法中没有 `逻辑非`，所以我在StackOverflow上找到了大家一般是这么做的：if后面直接接else. 相当的ugly，但是it works!
