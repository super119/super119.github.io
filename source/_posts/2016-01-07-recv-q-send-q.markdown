---
layout: post
title: "netstat -na输出中Recv-Q和Send-Q两项的值不为零需要注意程序是否有问题"
date: 2016-01-07 10:59
comments: true
categories: linux
---

Recv-Q：表示收到的数据中还有多少没有被进程取走（通过recv）

Send-Q：表示需要发送的数据还有多少没有被发出

所以，一般来说这两个值都是0，如果不为0且持续增长，那就表明程序出现了问题。

比如Recv-Q的数字持续增长，表示没有进程去取这些收到的数据。比如使用select+recv来收数据的时候，由于select有1024这个限制，所以如果socket的FD大于1024的时候，就会导致这个socket FD上的数据不会被select检测到从而导致recv不会被调用。

所以，通过netstat的这两个值就可以简单判断程序收不到包到底是包没到还是包没有被进程recv。
