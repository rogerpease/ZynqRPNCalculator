# ZynqRPNCalculator


Please see my website https://rogerpease.com/index.php/zynq-7000-rpn-calculator-with-kernel-driver/ for more details on this. 

Mainly I wanted to play with building a kernel module for an FPGA Fabric IP. 

The reader's digest version is there are three main steps:

1) Build the Vivado IP. If you want to skip that step you can pull the 
     IP I list in the python/RunRPNSanity.py

2) Build the kernel module. You'll probably need to edit my Makefile and 
  get the kernel (see https://discuss.pynq.io/t/how-to-compile-kernel-module-for-pynq-v3/5870/2) and cross-compile.  

3) Instance the kernel module and run. See the kernel/src for that. 
