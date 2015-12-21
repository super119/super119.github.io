---
layout: post
title: "Kernel Submitting Patches，有关ifdef和static inline & macro"
date: 2015-12-21 14:19
comments: true
categories: linux-kernel
---

From kernel: Documentation/SubmittingPatches
 
<!-- more -->

2) #ifdefs are ugly

Code cluttered with ifdefs is difficult to read and maintain.  Don't do
it.  Instead, put your ifdefs in a header, and conditionally define
'static inline' functions, or macros, which are used in the code.
Let the compiler optimize away the "no-op" case.

Simple example, of poor code:

``` cpp
        dev = alloc_etherdev (sizeof(struct funky_private));
        if (!dev)
                return -ENODEV;
        #ifdef CONFIG_NET_FUNKINESS
        init_funky_net(dev);
        #endif
```

Cleaned-up example:

``` cpp
(in header)
        #ifndef CONFIG_NET_FUNKINESS
        static inline void init_funky_net (struct net_device *d) {}
        #endif
(in the code itself)
        dev = alloc_etherdev (sizeof(struct funky_private));
        if (!dev)
                return -ENODEV;
        init_funky_net(dev);
```

3) 'static inline' is better than a macro

Static inline functions are greatly preferred over macros.
They provide type safety, have no length limitations, no formatting
limitations, and under gcc they are as cheap as macros.
Macros should only be used for cases where a static inline is clearly
suboptimal [there are a few, isolated cases of this in fast paths],
or where it is impossible to use a static inline function [such as
string-izing].

'static inline' is preferred over 'static __inline__', 'extern inline',
and 'extern __inline__'.
