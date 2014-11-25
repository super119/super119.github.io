---
layout: post
title: "Thunderbird 80(72) column - 邮件在80(72)列处line break"
date: 2014-11-25 11:13
comments: true
categories: linux linux-kernel
---

这个需求做linux kernel upstream的人都懂的......要设置Thunderbird在80列正确换行，只需要这样：

<!-- more -->

- Thunderbird如果撰写的邮件是plain text格式（在这里设置：Edit -> Account Settings -> Composition & Addressing, you should uncheck Compose in HTML format for all accounts），那么自动就会在72列处做line wrap。但是需要注意的是，这里的line wrap只是在display邮件内容和撰写邮件时，你可以看到。当你发出邮件的时候，这个line wrap就没有了，换句话说，别人看到的你的邮件内容，还是很长的一行行的文字。所以需要下面一步。

- 以下内容来自：

[http://arapulido.com/2009/12/01/enabling-line-wrapping-in-thunderbird/#comment-807](http://arapulido.com/2009/12/01/enabling-line-wrapping-in-thunderbird/#comment-807)

下文中提到的option设置在：Edit -> Preferences -> Advanced (General tab) -> Configure Editor：

> I am a happy user of the daily build of Thunderbird 3. Although it is still under development, is the best email client for Linux I’ve tried so far, at least, the one that better works for me. Email search & filtering is fast and reliable.
> 
> The only thing I don’t like is that wrapping your emails to 72 characters when sending plain text is not straightforward. Once you have selected to send your emails as plain text and have set the line wrap option to 72, there is still one more option to tweak.
> 
> Thunderbird has a “feature” that sends by default all plain text with the option format=flowed, which unwraps the email in the client receiver. To turn this feature off, you have to toggle one of the options in the chrome editor:
> 
> To disable flowed paragraphs, enforcing line breaks as formatted in the message, set the preference:
> 
> mailnews.display.disable_format_flowed_support true
> (http://kb.mozillazine.org/Mail_content_types#Plain_text)
> 
> To disable paragraph flow when you send plain text messages, and in the plain text part of multipart messages, set the preference:
> 
> mailnews.send_plaintext_flowed false
> (http://kb.mozillazine.org/Mail_content_types#Disabling_paragraph_flow)
> 
