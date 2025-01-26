# ZynqRPNCalculator


Please see my website https://rogerpease.com/index.php/zynq-7000-rpn-calculator-with-kernel-driver/ for more details on this. 

Mainly I wanted to play with building a kernel module for an FPGA Fabric IP. 

The reader's digest version is there are three main steps:

1) Build the Vivado IP. If you want to skip that step you can pull the 
     IP I list in the python/RunRPNSanity.py

   I had to run top-level tests with iverilog because I couldn't get vcd working properly with verilator

cd verilog/src; iverilog ../tb/tb_ZynqRPNCalculator_v1_0.v ZynqRPNCalculator_v1_0.v ZynqRPNCalculator_v1_0_S00_AXI.v ZynqRPNCalculator.v

2) Build the kernel module. You'll probably need to edit my Makefile and 
  get the kernel (see https://discuss.pynq.io/t/how-to-compile-kernel-module-for-pynq-v3/5870/2) and cross-compile.  

3) Instance the kernel module and run. See the kernel/src for that. 



