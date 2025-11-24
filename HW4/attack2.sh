#!/bin/sh

# Make the device node
mknod /dev/mytimer c 61 0
# Load the kernel module
insmod liamh_lab3/km/mytimer.ko

liamh_lab3/ul/ktimer -s 2 "Timer 1" &
liamh_lab3/ul/ktimer -s 2 "Timer 2" &
liamh_lab3/ul/ktimer -s 2 "Timer 3" &
liamh_lab3/ul/ktimer -s 2 "Timer 4" &
liamh_lab3/ul/ktimer -s 2 "Timer 5" &
liamh_lab3/ul/ktimer -s 2 "Timer 6" &
