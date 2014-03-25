---
layout: post
title: "给Mac OS X上的Chrome添加SSL证书"
date: 2014-03-25 21:49
comments: true
categories: MacOSX
---

Chrome在访问自签名SSL证书的网站时，会给出一个warning的页面，每次都要去点一下那个 `Proceed` 按钮特烦。每天点一次，日积月累下来每年也要花不少时间来点这个破玩意。Chrome就不能聪明一点来个诸如”记住我的选择“这样的按钮么？

折腾总是发生在忍无可忍的时候，幸好要解决这个问题不麻烦。

<!-- more -->

- 首先就是把SSL证书导出了。这个非常容易，访问一下有SSL warning的那个网站，在Chrome给出的warning页面，地址栏的开头有一个被划了斜线的https，点那里就可以看到证书的信息，从而可以将证书导出出来。

- 然后就是导入这个证书了。直接在Mac OS X的应用列表中启动 `Keychain Access`，当然你也可以在Chrome里面选择Settings -> Advanced settings -> Manage Certification，这样出来的就是Mac OS X的 `Keychain Access` 这个应用程序。

![Keychain Access](/downloads/image/keychain-access.png)

- 在 `Keychain Access` 的界面中，点击左下的 + 按钮，选择刚才得到的SSL证书，就将该证书导入了。

- 最后我们要选择 `Always Trust` 这个证书，否则SSL warning还是会有。在 `Keychain Access` 中，双击刚才导入的证书，在出现的对话框中，展开 `Trust` 那个部分，然后选择 `Always Trust` 就可以了。比如下图就是12306网站的SSL证书：

![Always trust SSL certification](/downloads/image/keychain-always-trust.png)

- 重启Chrome，再访问那个网站，就没有SSL warning了。
