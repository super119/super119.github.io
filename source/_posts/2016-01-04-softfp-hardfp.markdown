---
layout: post
title: "Compile: what is softfp & hardfp?"
date: 2016-01-04 09:56
comments: true
categories: linux
---

hardfp compiles inline FP instructions.  If you have an FPU this will be fastest.  If you don't, every FP instruction will trap to an emulation routine (assuming your runtime supports this).

<!-- more -->

softfp will compile a library call for every FP operation.  Use this if you never run on a system with an FPU.


来自gcc的manual：

> -mfloat-abi=name       Specifies which floating-point ABI to use.  Permissible values are: soft, softfp and hard.
> 
> Specifying soft causes GCC to generate output containing library calls for floating-point operations.  softfp allows the generation of code using hardware floating-point instructions, but still uses the soft-float calling conventions.  hard allows generation of floating-point instructions and uses FPU-specific calling conventions.
> 
> The default depends on the specific target configuration.  Note that the hard-float and soft-float ABIs are not link-compatible; you must compile your entire program with the same ABI, and link with a compatible set of libraries.
