---
layout: post
title: "Top中是如何取到Linux内核中的Hertz的？CPU使用率到底是怎么算出来的？"
date: 2016-02-15 10:29
comments: true
categories: linux-kernel
---

首先来看CPU使用率到底是怎么算出来的。依据的是这个公式：

<!-- more -->

``` bash
(process jiffies) * 100.0f / ((float)Hertz * (float)et * (Rc.mode_irixps ? 1 : Cpu_tot));
```

process jiffies是内核提供的该进程在DeltaT时间内消耗的jiffies。具体是/proc/<pid>/stat文件的第14-17 token。14－17token分别是utime, stime, cutime, cstime。cutime/cstime分别是该进程spawn的子进程在用户态和内核态消耗的jiffies。/proc/<pid>/task目录下有该进程所有线程的数据，也是在stat文件中读取，格式和process的是一样的，因为Linux内核中线程和进程区别不大的。所以 也完全可以将该进程所有线程消耗的jiffies累加起来，从而得到该进程的jiffies。man proc可以看到/proc下所有内容的解释。

注意stat中的jiffies是一个绝对累计值，所以要取两个时间点，算DeltaT中消耗的jiffies。

那 什么是jiffies呢？其实就是Linux内核定义的一个时间单位，值就是1/Hertz。Linux内核中，进程/线程消耗的时间，单位都是这个 jiffies。Linux内核没有开放什么系统调用，让程序可以直接取得这个Hertz，从而算出jiffies，为此，Top自己写了一个函数来算 Hertz这个值（不同的硬件平台，这个Hertz是不一样的。而且这个jiffies就是Linux内核调度进程的时间片大小）。最后会给出这个 Hertz在top中是如何计算出来的函数。

OK，回到上面的公式。et表示流逝的时间，单位是秒。最后一个部分是CPU/core数。

所以，总结来看，CPU使用率其实就是在DeltaT时间内，进程一共消耗了多少jiffies，消耗的越多，自然该进程的CPU使用率就越高了。更抽象一点来说，一个进程在单位时间内，被分配到的时间片越多，那么这个进程的CPU占用率就越高。

最后给出Top中是如何计算这个Hertz的，一般现在这个Hertz都是100，也就是jiffies就代表10ms，这也是linux内核中调度时间片的大小。

``` cpp
    /***********************************************************************
    * Some values in /proc are expressed in units of 1/HZ seconds, where HZ
    * is the kernel clock tick rate. One of these units is called a jiffy.
    * The HZ value used in the kernel may vary according to hacker desire.
    * According to Linus Torvalds, this is not true. He considers the values
    * in /proc as being in architecture-dependant units that have no relation
    * to the kernel clock tick rate. Examination of the kernel source code
    * reveals that opinion as wishful thinking.
    *
    * In any case, we need the HZ constant as used in /proc. (the real HZ value
    * may differ, but we don't care) There are several ways we could get HZ:
    *
    * 1. Include the kernel header file. If it changes, recompile this library.
    * 2. Use the sysconf() function. When HZ changes, recompile the C library!
    * 3. Ask the kernel. This is obviously correct...
    *
    * Linus Torvalds won't let us ask the kernel, because he thinks we should
    * not know the HZ value. Oh well, we don't have to listen to him.
    * Someone smuggled out the HZ value. :-)
    *
    * This code should work fine, even if Linus fixes the kernel to match his
    * stated behavior. The code only fails in case of a partial conversion.
    *
    * Recent update: on some architectures, the 2.4 kernel provides an
    * ELF note to indicate HZ. This may be for ARM or user-mode Linux
    * support. This ought to be investigated. Note that sysconf() is still
    * unreliable, because it doesn't return an error code when it is
    * used with a kernel that doesn't support the ELF note. On some other
    * architectures there may be a system call or sysctl() that will work.
    */
    unsigned long long Hertz;
    static void old_Hertz_hack(void){
      unsigned long long user_j, nice_j, sys_j, other_j;  /* jiffies (clock ticks) */
      double up_1, up_2, seconds;
      unsigned long long jiffies;
      unsigned h;
      char *restrict savelocale;
      savelocale = setlocale(LC_NUMERIC, NULL);
      setlocale(LC_NUMERIC, "C");
      do{
        FILE_TO_BUF(UPTIME_FILE,uptime_fd);  sscanf(buf, "%lf", &up_1);
        /* uptime(&up_1, NULL); */
        FILE_TO_BUF(STAT_FILE,stat_fd);
        sscanf(buf, "cpu %Lu %Lu %Lu %Lu", &user_j, &nice_j, &sys_j, &other_j);
        FILE_TO_BUF(UPTIME_FILE,uptime_fd);  sscanf(buf, "%lf", &up_2);
        /* uptime(&up_2, NULL); */
      } while((long long)( (up_2-up_1)*1000.0/up_1 )); /* want under 0.1% error */
      setlocale(LC_NUMERIC, savelocale);
      jiffies = user_j + nice_j + sys_j + other_j;
      seconds = (up_1 + up_2) / 2;
      h = (unsigned)( (double)jiffies/seconds/smp_num_cpus );
      /* actual values used by 2.4 kernels: 32 64 100 128 1000 1024 1200 */
      switch(h){
      case    9 ...   11 :  Hertz =   10; break; /* S/390 (sometimes) */
      case   18 ...   22 :  Hertz =   20; break; /* user-mode Linux */
      case   30 ...   34 :  Hertz =   32; break; /* ia64 emulator */
      case   48 ...   52 :  Hertz =   50; break;
      case   58 ...   61 :  Hertz =   60; break;
      case   62 ...   65 :  Hertz =   64; break; /* StrongARM /Shark */
      case   95 ...  105 :  Hertz =  100; break; /* normal Linux */
      case  124 ...  132 :  Hertz =  128; break; /* MIPS, ARM */
      case  195 ...  204 :  Hertz =  200; break; /* normal << 1 */
      case  253 ...  260 :  Hertz =  256; break;
      case  393 ...  408 :  Hertz =  400; break; /* normal << 2 */
      case  790 ...  808 :  Hertz =  800; break; /* normal << 3 */
      case  990 ... 1010 :  Hertz = 1000; break; /* ARM */
      case 1015 ... 1035 :  Hertz = 1024; break; /* Alpha, ia64 */
      case 1180 ... 1220 :  Hertz = 1200; break; /* Alpha */
      default:
    #ifdef HZ
        Hertz = (unsigned long long)HZ;    /* <asm/param.h> */
    #else
        /* If 32-bit or big-endian (not Alpha or ia64), assume HZ is 100. */
        Hertz = (sizeof(long)==sizeof(int) || htons(999)==999) ? 100UL : 1024UL;
    #endif
        fprintf(stderr, "Unknown HZ value! (%d) Assume %Ld.\n", h, Hertz);
      }
    }
```

代码很简单，其实就是计算单位1秒内，每CPU/core产生了多少了jiffies。这个方法就和具体的硬件平台没有关系了。所以这是一个取得Linux内核Hertz的好方法。
