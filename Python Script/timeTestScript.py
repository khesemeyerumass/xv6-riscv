# Kyle Hesemeyer

import pexpect
import os
import subprocess
import time

# Script I used to run the time test programs that outputs data regarding the time it 
# takes for each allocator to allocate and free memory. 
xv6_process = pexpect.spawn("make qemu")
fout = open('mylog.txt','wb')
xv6_process.logfile = fout
xv6_process.expect_exact("init: starting sh")
xv6_process.expect_exact("$ ")

# Test used to record the times of malloc and free
i = 1
while (i <= 250):
    start = time.time()
    xv6_process.sendline('allocTimeTest {}'.format(i))
    xv6_process.expect_exact("$ ")
    end = time.time()
    t = end - start
    print("{}".format(t))
    i += 1
    
xv6_process.close()
