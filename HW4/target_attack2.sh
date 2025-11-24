#!/bin/sh

# Make the device node
mknod /dev/mytimer c 61 0
# Load the kernel module
insmod $1/mytimer.ko

$1/ktimer -s 2 "Timer 1" &
$1/ktimer -s 2 "Timer 2" &
$1/ktimer -s 2 "Timer 1" &
$1/ktimer -s 2 "Timer 1" &
$1/ktimer -s 2 "Timer 2" &
$1/ktimer -s 2 "Timer 2" &
