---
layout: post
title: "在博客内容中插入Liquid代码时的注意点"
date: 2013-11-07 09:44
comments: true
categories: Blog
---

Octopress/Jekyll一个很好的功能就是可以非常方便的插入代码，深得码农的喜爱。不过这里有一个注意点就是，如果你插入的代码中含有Liquid代码，那很可能会导致编译出错，比如：

<!-- more -->

``` bash rake generate出错信息
## Generating Site with Jekyll
unchanged sass/screen.scss
Configuration from /home/markz/Projects/blog/_config.yml
Building site: source -> public
Liquid Exception: if tag was never closed in 2013-11-05-add-uyan-comment.markdown
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/block.rb:88:in `assert_missing_delimitation!'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/block.rb:49:in `parse'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/tag.rb:10:in `initialize'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/tags/if.rb:24:in `initialize'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/block.rb:28:in `new'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/block.rb:28:in `parse'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/document.rb:5:in `initialize'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/template.rb:58:in `new'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/template.rb:58:in `parse'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/liquid-2.3.0/lib/liquid/template.rb:46:in `parse'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/jekyll-0.12.0/lib/jekyll/convertible.rb:79:in `do_layout'
/work/markz/Projects/blog/plugins/post_filters.rb:167:in `do_layout'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/jekyll-0.12.0/lib/jekyll/post.rb:195:in `render'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/jekyll-0.12.0/lib/jekyll/site.rb:200:in `block in render'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/jekyll-0.12.0/lib/jekyll/site.rb:199:in `each'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/jekyll-0.12.0/lib/jekyll/site.rb:199:in `render'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/jekyll-0.12.0/lib/jekyll/site.rb:41:in `process'
/home/markz/.rbenv/versions/1.9.3-p0/lib/ruby/gems/1.9.1/gems/jekyll-0.12.0/bin/jekyll:264:in `<top (required)>'
/home/markz/.rbenv/versions/1.9.3-p0/bin/jekyll:23:in `load'
/home/markz/.rbenv/versions/1.9.3-p0/bin/jekyll:23:in `<main>'
Build Failed
```

原因地球人都知道，就是博客中插入的代码含有Liquid的if标签。解决办法有两种：

1. 使用{% raw %}`{% include_code %}`{% endraw %}。将代码存成一个文件，然后将其include到博客中来。

2. 使用raw标签，将代码（包含三个backticks）使用`{{ "{% raw " }}%}`，`{{ "{% endraw " }}%}`包起来即可。


**Note:** 上文中要将raw和endraw打印出来需要将其转义，其他标签都可以用`raw/endraw`搞定，但是要转义他们本身，要这么写：


{% raw %}
``` bash
{{ "{% raw " }}%} {{ "{% endraw " }}%}
```
{% endraw %}
