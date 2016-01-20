---
layout: post
title: "Fortran90 代码书写注意点"
date: 2016-01-20 15:28
comments: true
categories: HPC
---

if语句后面的条件表达式一定要用（）括 起来，如 

``` cpp
if ( salary < 1000 ) then 
if ( (salary >= 1000) .AND. ( salary <= 5000) ) then 
```

没有括号的话编译会产生错误 

<!-- more -->

****

if－else if从句不用end if语句。也就是说，虽然 `if－else if` 这个从句中含有两个if判断，但在最后条件判断结束后，只需要写一个 `end if` 就OK了，不用写多个，如： 

``` cpp
if ( salary < 1000 ) then 
! do something 
else if ( salary > 5000) then 
! do something 
end if
```

****

如果if语句后只有一句代码，那么要注意then和end if都应该省略不写，如 `if (salary < 1000) exit !` 没有then和end if，如果exit写在下一行，那么就应该写上then和end if 

****

DO循环中：`do counter=1, 10, 1 ` 需要注意的是这里的第二个参数不要写成counter<=10 

Fortran中自动会添加counter<=这样的描述 

如果要实现递减的循环，可以将递变参数改成负数 

****

Fortran中使用write可以控制格式化的输出，如write(*, 10)表示取行号为10行的FORMAT语句中的格式化，但一般来说，我们会写在write语句中，这样可读性好，此时的write应该这样写： 

``` cpp
write(*, "('Hello World! This is ', I2, ' Processes!')") process_number 
```

也就是说，首先要用"( )"将格式化字符串包起来，然后在中间，常量字符串用单引号包住，格式控制代码使用,号分割开
 
***

Fortran中非常容易混淆的两个元素是“字符串”和“字符数组”！这样声明的是一个字符串： 

character(len=20) string

而这样声明的，就是一个字符数组：

character :: string(20)

对于字符串，是不可以通过string(i)这样来引用元素的，因为这是数组的做法，对于字符串来说， 

要想取得其中的一个或部分元素，需要使用string(i:i)或string(i:i+4)这样的代码 

对于数组来说，上面的方法一样可以用，另外，还可以通过string(i)这样的方法来直接引用其中的元素
 
****

Fortran的所有变量声明都必须放在代码的一开始，不可以在某句代码之后又出现变量的定义，这和C是相同的。但对于C来说，gcc编译器在这一点上做的比较好，gcc编译器允许c代码的变量随处定义，并且允许使用//而不是/* */来注释一行。注释使用！符号 

****

一行写不完的代码，在行末加上 &，表示下一行和本行是一行代码；也可以在某行的开头使用 &，表示，本行和上一行是一行代码
