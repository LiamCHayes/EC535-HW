#!/bin/sh

# Make the device node
mknod /dev/mytimer c 61 0
# Load the kernel module
insmod $1/mytimer.ko

# Try to access data from a null pointer
$1/ktimer -s 10 "Timer 1" &
$1/ktimer -r
$1/ktimer -r
$1/ktimer -r
$1/ktimer -r
$1/ktimer -r
$1/ktimer -r
$1/ktimer -r
$1/ktimer -r
$1/ktimer -r
