#!/bin/sh

natd -same_ports -use_sockets -unregistered_only -dynamic -interface ppp0 -clamp_mss
ipfw -f flush
ipfw add divert natd ip from any to any via ppp0
ipfw add pass all from any to any
sysctl -w net.inet.ip.forwarding=1
