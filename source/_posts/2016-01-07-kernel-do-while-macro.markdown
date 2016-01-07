---
layout: post
title: "为什么包含多句代码的宏要用do while包括起来？"
date: 2016-01-07 11:05
comments: true
categories: linux-kernel
---

为了看起来更清晰，这里用一个简单点的宏来演示：

``` cpp
#define SAFE_DELETE(p) do{ delete p; p = NULL} while(0)
```

<!-- more -->

假设这里去掉 `do...while(0)`,

``` cpp
#define SAFE_DELETE(p) delete p; p = NULL;
```

那么以下代码：

``` cpp
if(NULL != p) SAFE_DELETE(p)
else   ...do sth...
```

就有两个问题，

1. 因为if分支后有两个语句，else分支没有对应的if，编译失败

2. 假设没有else, SAFE_DELETE中的第二个语句无论if测试是否通过，会永远执行。

你可能发现，为了避免这两个问题，我不一定要用这个令人费解的do...while,  我直接用{}括起来就可以了

``` cpp
#define SAFE_DELETE(p) { delete p; p = NULL;}
```

的确，这样的话上面的问题是不存在了，但是我想对于C++程序员来讲，在每个语句后面加分号是一种约定俗成的习惯，这样的话，以下代码:

``` cpp
if(NULL != p) SAFE_DELETE(p);
else   ...do sth...
```

其else分支就无法通过编译了（原因同上），所以采用 `do...while(0)` 是做好的选择了。

也许你会说，我们代码的习惯是在每个判断后面加上{}, 就不会有这种问题了，也就不需要do...while了，如：

``` cpp
if(...) 
{
}
else
{
}
```

诚然，这是一个好的，应该提倡的编程习惯，但一般这样的宏都是作为library的一部分出现的，而对于一个library的作者，他所要做的就是让其库具有通用性，强壮性，因此他不能有任何对库的使用者的假设，如其编码规范，技术水平等。
