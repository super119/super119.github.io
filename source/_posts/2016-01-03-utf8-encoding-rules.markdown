---
layout: post
title: "UTF-8 Encoding rules"
date: 2016-01-03 11:38
comments: true
categories: linux
---

``` cpp
/* UTF-8 encoding
00000000-01111111  00-7F  US-ASCII (single byte)
10000000-10111111  80-BF  Second, third, or fourth byte of a multi-byte sequence
11000000-11011111  C0-DF  Start of 2-byte sequence
11100000-11101111  E0-EF  Start of 3-byte sequence
11110000-11110100  F0-F4  Start of 4-byte sequence
*/
```
