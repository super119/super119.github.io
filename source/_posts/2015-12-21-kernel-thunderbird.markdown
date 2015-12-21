---
layout: post
title: "Thunderbird配置 - for kernel maillist"
date: 2015-12-21 14:22
comments: true
categories: linux-kernel
---

From kernel: Documentation/email-clients.txt
 
<!-- more -->

Thunderbird (GUI)

Thunderbird is an Outlook clone that likes to mangle text, but there are ways
to coerce it into behaving.

- Allows use of an external editor:
  The easiest thing to do with Thunderbird and patches is to use an
  "external editor" extension and then just use your favorite $EDITOR
  for reading/merging patches into the body text.  To do this, download
  and install the extension, then add a button for it using
  View->Toolbars->Customize... and finally just click on it when in the
  Compose dialog.

To beat some sense out of the internal editor, do this:

- Edit your Thunderbird config settings so that it won't use format=flowed.
  Go to "edit->preferences->advanced->config editor" to bring up the
  thunderbird's registry editor, and set "mailnews.send_plaintext_flowed" to
  "false".

- Disable HTML Format: Set "mail.identity.id1.compose_html" to "false".

- Enable "preformat" mode: Set "editor.quotesPreformatted" to "true".

- Enable UTF8: Set "prefs.converted-to-utf8" to "true".

- Install the "toggle wordwrap" extension.  Download the file from:
    https://addons.mozilla.org/thunderbird/addon/2351/
  Then go to "tools->add ons", select "install" at the bottom of the screen,
  and browse to where you saved the .xul file.  This adds an "Enable
  Wordwrap" entry under the Options menu of the message composer.
