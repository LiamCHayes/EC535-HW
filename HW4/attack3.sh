#!/bin/sh

# Make the device node
mknod /dev/mytimer c 61 0
# Load the kernel module
insmod liamh_lab3/km/mytimer.ko

# Try to access data from a null pointer
liamh_lab3/ul/ktimer -s 2 "Timer 1" &
liamh_lab3/ul/ktimer -r
liamh_lab3/ul/ktimer -l
cat /proc/mytimer
liamh_lab3/ul/ktimer -s 2 "Timer 1" &
liamh_lab3/ul/ktimer -r
liamh_lab3/ul/ktimer -l
cat /proc/mytimer
liamh_lab3/ul/ktimer -s 2 "Timer 1" &
liamh_lab3/ul/ktimer -r
liamh_lab3/ul/ktimer -l
cat /proc/mytimer
liamh_lab3/ul/ktimer -s 2 "Timer 1" &
liamh_lab3/ul/ktimer -r
liamh_lab3/ul/ktimer -l
cat /proc/mytimer



