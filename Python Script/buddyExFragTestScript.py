# Kyle Hesemeyer

import pexpect
import os
import subprocess

# Script I used to run the external fragmentation test program that outputs data 
# regarding the external fragmentation of the memory allocator that was compiled.
# Note: This is only intended for the buddy memory allocator
xv6_process = pexpect.spawn("make qemu")
fout = open('mylog.txt','wb')
xv6_process.logfile = fout
xv6_process.expect_exact("init: starting sh")
xv6_process.expect_exact("$ ")

# Test used to record Internal Fragmentation
xv6_process.sendline('allocExFragTest 1')
xv6_process.expect_exact("$ ")
xv6_process.close()
