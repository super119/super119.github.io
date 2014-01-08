---
layout: post
title: "OpenGL Programming on MacOS X (Mavericks)"
date: 2014-01-06 21:30
comments: true
categories: OpenGL
---

最近开始看《OpenGL Programming Guide 8th Edition》一书。所以很自然的，就会想把书中的源代码在MacOS X Mavericks上试验一遍。很显然程序是没法在Mavericks上run的，所以就对程序做了一些修改。在试验的过程中，自然也发现了一些书中没有说到的内容，所以记录下来。

事实上，这本书中的代码是在Windows上测试的，所以MacOS X上就会有一些不同。尤其体现在一些第三方的framework/library上。

本文是在试验该书中第一个源程序的时候，发现的一些东西。和我们第一个编写的HelloWorld程序一样，我称该程序为HelloGL。

<!-- more -->

为了方便阅读，所有的代码在最后给出。系统的环境是Mavericks + Xcode 4.6.1。这里说一下我在原书给出的代码上，做出的一些修改：

## 有关GLEW ##

GLEW当然是可以使用的，只是不像GLUT，Mavericks没有自带GLEW，需要自己下载安装。不过安装也超级简单：

``` bash
make glew.lib
sudo make install
```

我尝试过直接make，也就是编译所有的内容（其中包括glewinfo等实用程序），在Mavericks上会编译失败（提示缺少AGL/agl.h）。所以我只编译了glew的库，事实上我们也只需要库就可以了。

有了GLEW之后，只需要include一个 `GL/glew.h` 就搞定了。因为GLEW会将OpenGL中用到的所有的数据类型和函数都定义一遍，而且提供大量的宏，使我们可以查询一个具体的 `feature/函数` 在当前平台上是否存在。这也是GLEW的主要功能了。对于函数，GLEW会根据不同平台，去打开对应的库，取出对应函数的地址，从而让我们可以调用到该平台上正确的实现。参看书中的附录F：`OpenGL and Window Systems`，有详细的解释。

不过StackOverflow上说，其实在MacOS X中，GLEW并不需要。我试验了一下，的确是不需要（当然我只是用本文中的这个简单的程序试验了一下）。理由是：

> About glew, normally you don't require glew on MacOS due the way how the OpenGL layer is implemented in MacOS. You are restricted to the OpenGL features MacOS provides/exposes. So either the features are available via the headers of MacOS or not.

> The header would be #include <OpenGL/gl3.h> where also the naming is missleading, it does not mean only OpenGL 3, it is the same like with the context.

## 主程序的改动 ##

主要改动了以下几点：

- 去掉了 `include "vgl.h"`。因为这个头文件中啥实在东西没有，唯一一个需要用到的就是一个 `BUFFER_OFFSET` 宏的定义，把它拿到主程序文件中来了。

- 代码中涉及的两个shader程序：`triangle.vert` 和 `triangle.frag`，改名成了 `simple.vert` 和 `simple.frag`。一来是感觉更贴切，二来是这两个shader程序在该书的源代码包中有，拷贝过来就能用了。

- 如上面所说，GLEW不是必须的，所以我去掉了GLEW相关的代码。

- 在main函数中增加了一行，用来打印当前GLSL的版本：

``` cpp
cout << "Supported GLSL version is: " <<
	glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
```

因为后续也会提到，一开始运行程序的时候，会发现shader程序无法编译，所以增加了这一行打印，方便调试。这个也是来自StackOverflow上大佬们的建议。

- 去掉了main函数中这两行代码：

``` cpp
glutInitContextVersion(4, 3);
glutInitContextProfile(GLUT_CORE_PROFILE);
```

理由是，我没有使用书中建议的 `freeglut`。因为Mavericks，或者准确点来说Xcode自带了GLUT（应该是openglut），所以就没有必要再安装一个类似的玩意了。
而Xcode自带的GLUT，是没有上述的两个函数的。

- 最关键的一点改动：

``` cpp
glutInitDisplayMode(GLUT_RGBA);
```

改成：

``` cpp
glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA);
```

正是由于改动#4，导致程序运行的时候，没有正确的选择OpenGL的版本和profile。所以如果没有这个改动，程序运行会发现shader程序没法编译，错误信息是：`unsupported version xxx`（shader程序的第一行就是#version）。所以我很小白的在StackOverflow上问了这个问题，最终得到了大佬的回答：

> MacOS uses Legacy Profile as default for all created OpenGL context. Therefor by default only OpenGL up to 2.1 and GLSL up to 1.20 is supported.

> To use OpenGL 3.2+ you need to switch to the Core Profile. The naming here is a little bit confusing because it stats only 3.2Core profile, but actually this 3.2 or later (every OpenGL profile that is supported by the system/driver that is backwards compatible to 3.2)

> For glut (depends on the version of glut if it works) the command on MacOS is:

> glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | ...  ) Where | ... would be the other options you want to pass to glutInitDisplayMode.

正如上面所说，虽然名字叫3_2_CORE_PROFILE，但是其实会使用系统当前最新版本的OpenGL实现啦。上面GLEW一节中也说道，虽然include的头文件名字叫 `OpenGL/gl3.h`，但其实不是真的只是OpenGL 3啦。

## LoadShaders的改动 ##

LoadShaders是书中提供的一个用来读取、编译shader程序的一小段代码。这部分内容我几乎没有改动，除了：

- 去掉了 `#include <GL/gl.h>` 等，改成了 `#include <OpenGL/gl3.h>`，当然如果你用GLEW，也可以改成：`#include <GL/glew.h>`。

- 将LoadShader.cpp中的 `_DEBUG` 改成了 `DEBUG`。理由是：Xcode的Project中，如果是编译运行Debug版本，默认就会定义 `DEBUG` 这个宏，所以做这个修改就省的以后每次新建项目，都要再去定义 `_DEBUG` 了。

## 其他一些Tips ##

代码的改动就是上述的一些内容了。不过为了让程序能在Xcode中编译和运行，还需要做一些工作。这里就作为Tips来描述一下。

注：其实也是没事找事，在终端下自己写一个Makefile就可以搞定的事情，偏要用Xcode。只是自从用了MacOS X之后，还没有用过Xcode，想尝试一下而已。

- 有关GLEW的头文件。Xcode中的Project，有一个配置项叫做：`Always Search User Paths`。需要将这个改成Yes，并将 `User Header Search Paths` 设置成 `/usr/include`。否则代码编译的时候就会错误：找不到 `GL/glew.h`。从Linux编程过来的人表示极度不适应...

- GLEW的库文件。和Linux下的C编程一样，我们都知道需要在编译的时候给出一个链接的参数：`-lGLEW`。这可以在Xcode中Project设定的 `Other Linker Flags` 中设置。但是设置完了之后，再次报告链接错误：找不到libGLEW，但是文件明明就在 `/usr/lib` 目录下。从Linux编程过来的人再次表示极度不适应... 最后没办法，只能这样：

``` bash
sudo ln -s /usr/lib/libGLEW.dylib /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/usr/lib/libGLEW.dylib
```

所以到了这里，可以基本明白了，Xcode是一个相对独立而且封闭的环境，所有的dependencies都被约束在了Xcode的体系下。我能再次吐槽么？

- 如果要在Xcode中run/debug程序的话，两个shader文件：`simple.vert` 和 `simple.frag` 要放到Xcode运行程序的working directory中，也就是这个目录：

``` bash
/Users/mark/Library/Developer/Xcode/DerivedData/HelloGL-<debug hash id>/Build/Products/Debug
```

记得我们在主程序中添加的一行用来打印当前GLSL的版本的代码么？这里派上用处了，将你的shader程序中的 `#version xxx` 修改成这里打印出来的值，因为这才是你的系统当前支持的GLSL的版本。

- 在Xcode Project设定中，将GLUT和OpenGL添加到 `Link Binary with Libraries` 中去。这个用过Xcode的都应该知道：

![HelloGL Link Binary with Libraries](/downloads/image/HelloGL-link-libs.png)

整个项目的文件列表是长这个样子的：

![HelloGL files](/downloads/image/HelloGL-files.png)

## 代码 ##

{% include_code HelloGL主程序 c++ HelloGL/main.cpp %}

{% include_code LoadShaders c++ HelloGL/LoadShaders.cpp %}

{% include_code LoadShaders头文件 cpp HelloGL/LoadShaders.h %}

{% include_code simple.vert cpp HelloGL/simple.vert %}

{% include_code simple.frag cpp HelloGL/simple.frag %}
