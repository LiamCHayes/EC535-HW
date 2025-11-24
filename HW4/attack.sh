#!/bin/sh

# Make the device node
mknod /dev/mytimer c 61 0
# Load the kernel module
insmod km/mytimer.ko

# Try to access data from a null pointer
ul/ktimer -s 2 "Timer 1" &
ul/ktimer -r
ul/ktimer -l
cat /proc/mytimer

# Create a race condition in a shared variable

# Buffer overflows in writing data 



