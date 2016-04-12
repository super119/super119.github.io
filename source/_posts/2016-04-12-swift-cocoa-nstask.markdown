---
layout: post
title: "NSTask Tips (swift)"
date: 2016-04-12 12:51
comments: true
categories: MacOSX
---

NSTask是常用的一个Foundation class，它相当于Linux中的popen，可以用来fork执行一个程序，然后将程序的输入输出重定向。然而我在使用这个class的时候，遇到了一些问题，记录在此。

<!-- more -->

## 如何取得程序的输出（比如stdout） ##

这个比较容易，比如：

``` java
let task = NSTask()
task.launchPath = xxx
task.arguments = xxx

let outPipe = NSPipe()
task.standardOutput = outPipe
let errPipe = NSPipe()
task.standardError = errPipe

outPipe.fileHandleForReading.readabilityHandler = {
    (handle: NSFileHandle) in
    let data = handle.readDataOfLength(128)
    <your String>.appendContentsOf(String(data: data, encoding: NSUTF8StringEncoding)!)
}

task.launch()
```

当然你也可以用NSPipe.fileHandleForReading的readToEndOfFile来一次性读出来（等待task执行完毕之后）。

## 如何知道程序执行结束 ##

根据NSTask的manual，有两种方法：

- 设置NSTask的terminationHandler

- 使用NSNotificationCenter，注册监听消息：NSTaskDidTerminateNotification

这两种方法都可以，但是有一些区别：

- terminationHandler是在NSTask的线程中被调用的，所以如果我们要在程序执行结束后去update UI，那么在这里做是不行的，运行的时候xcode会告诉你，UI被另外一个background thread修改，会出一个assertion。

- NSTaskDidTerminateNotification就没有上述的问题，因为我们注册的NSTaskDidTerminateNotification的observer就是在程序的主线程（main）被调用的。

我们可以通过在terminationHandler和NSTaskDidTerminateNotification observer函数中加入这样的代码来看出区别：

``` java
NSLog("Current thread is: %@", NSThread.currentThread())
```

通过打印当前thread的信息，就可以看出terminationHandler是在一条非main的线程中被call的，而notification则不是。

由此可以猜测，NSTask应该是首先create一条thread，然后在这条thread中去执行fork，最后调用类似waitpid这样的函数等待被执行的程序执行完毕，并且同时调用我们注册的terminationHandler。

需要注意的是，注册的notification observer一定要及时的remove（可以在observer function里面去remove），否则一旦你多注册一次，你的observer function就会被多call一次。很显然，notification center是用一张list来记录所有的observer的，当有notification发生时，这个list上所有的observer function都会被call一遍。

此外还有一点：如果我们有设置terminationHandler，那么NSTaskDidTerminateNotification就不会被发送了。

## 如何得知程序的return value ##

按照NSTask的manual，可以通过读取：terminationStatus来得知。但是我做下来发现这个property读取出来的值不一定正确。

也就是说，terminationStatus和我们手动执行：`run xxx program; echo $?` 这样出来的结果是不同的。

目前还不知道这个问题的原因和解决方法。

## 有关Swift的selector ##

在注册notification的时候，需要填写一个名为selector的param，也就是我们的notification observer的function。

在swift 2.1中，这里可以直接写function的名字（作为一个字符串），但是一定要注意和objective-C一样，function name是要带参数的，比如：`myNotificationObserverFunction:`，最后的冒号就表示这个function有一个参数。

漏写这个冒号就会导致runtime 程序无法找到这个function：unknown selector....

在Swift 2.2中，有一个新的语法可以显式的生成一个selector：`#selector(function name)`，不过貌似目前最新的Xcode还只支持到swift 2.1，所以目前还只能用上面的方法。
