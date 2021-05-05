# Kyle Hesemeyer

import pexpect
import os
import subprocess

# Script I used to run the internal fragmentation test program that outputs data 
# regarding the internal fragmentation of the memory allocator that was compiled.
# Note: It is only intended for the buddy memory allocator since the internal 
#       fragmentation of the storage allocator will always be zero.
xv6_process = pexpect.spawn("make qemu")
fout = open('mylog.txt','wb')
xv6_process.logfile = fout
xv6_process.expect_exact("init: starting sh")
xv6_process.expect_exact("$ ")

# Test used to record Internal Fragmentation
xv6_process.sendline('allocInFragTest')
xv6_process.expect_exact("$ ")
xv6_process.close()

