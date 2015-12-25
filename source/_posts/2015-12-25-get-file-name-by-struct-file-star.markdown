---
layout: post
title: "Kernel: 怎么通过struct file *得到文件名"
date: 2015-12-25 10:54
comments: true
categories: linux-kernel
---

只是一个小Tip，代码如下：

<!-- more -->

``` cpp
char *tmp;
char *pathname;

path_get(&file->f_path);

tmp = (char *)__get_free_page(GFP_TEMPORARY);
if (!tmp) {
    return -ENOMEM;
}

pathname = d_path(&file->f_path, tmp, PAGE_SIZE);
path_put(&file->f_path);

if (IS_ERR(pathname)) {
    free_page((unsigned long)tmp);
    return PTR_ERR(pathname);
}

printk(KERN_WARNING "File name: %s\n", pathname);
free_page((unsigned long)tmp);
```
