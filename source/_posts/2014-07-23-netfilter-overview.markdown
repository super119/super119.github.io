---
layout: post
title: "Linux netfilter概述，mangle/nat/filter, prerouting/input/forward...到底是啥"
date: 2014-07-23 18:30
comments: true
categories: network reprints
---

本文转载自：

[http://shorewall.net/NetfilterOverview.html](http://shorewall.net/NetfilterOverview.html)

<!-- more -->

Netfilter consists of three tables: Filter, Nat and Mangle. Each table has a number of build-in chains: PREROUTING, INPUT, FORWARD, OUTPUT and POSTROUTING.

Rules in the various tables are used as follows:

Filter
Packet filtering (rejecting, dropping or accepting packets)

Nat
Network Address Translation including DNAT, SNAT and Masquerading

Mangle
General packet header modification such as setting the TOS value or marking packets for policy routing and traffic shaping.

Raw
Used primarily for creating exemptions from connection tracking with the NOTRACK target. Also used for stateless DNAT.

Rawpost
Used for stateless SNAT.

The following diagram shows how packets traverse the various builtin chains within Netfilter. Note that not all table/chain combinations are used.

![Netfilter Overview](/downloads/image/netfilter-overview.png)


"Local Process” means a process running on the Shorewall system itself.

A more elaborate version of this flow is available here and this one contrasts the Netfilter flow with that of ipchains.

In the above diagram are boxes similar to this:

![Input chain](/downloads/image/netfilter-input-chain.png)

The above box gives the name of the built-in chain (INPUT) along with the names of the tables (Mangle and Filter) that the chain exists in and in the order that the chains are traversed. The above sample indicates that packets go first through the INPUT chain of the Mangle table then through the INPUT chain of the Filter table. When a chain is enclosed in parentheses, Shorewall does not use the named chain (INPUT) in that table (Mangle).

** Keep in mind that chains in the Nat table are only traversed for new connection requests (including those related to existing connections) while the chains in the other tables are traversed on every packet. **
