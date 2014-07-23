#!/bin/sh

killall natd
ipfw -f flush
sysctl -w net.inet.ip.forwarding=0
