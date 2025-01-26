#!/usr/bin/env python3 
#
# Quick sanity check to ensure my image works properly. 
#  Also get base address 
#
#
#


import sys
import os

os.environ["XILINX_XRT"] = "/usr"
sys.path.append('/usr/local/share/pynq-venv/lib/python3.10/site-packages')

if os.geteuid() != 0:
   print("Must be run as root");
   exit(1) 

IMAGE="pynq_z2_zynqrpn.bit"
HWH  ="pynq_z2_zynqrpn.hwh"
#
# I keep pynq-z2 copies of these images on 
#
#  https://rogerpease.com/pynqz2/pynq_z2_zynqrpn.bit 
#  https://rogerpease.com/pynqz2/pynq_z2_zynqrpn.hwh
#
if not os.path.exists(IMAGE) or not os.path.exists(HWH):
   print("Must copy the .bit file and .hwh file from your Vivado directory")
   exit(1) 
     



from pynq import Overlay
from pynq import allocate
 

def sanity_check(overlay):
   overlay.ZynqRPNCalculator_v1_0.write(0x4,0x1)
   overlay.ZynqRPNCalculator_v1_0.write(0x0,0x123)
   overlay.ZynqRPNCalculator_v1_0.write(0x4,0x2)
   overlay.ZynqRPNCalculator_v1_0.write(0x0,0x456)
   overlay.ZynqRPNCalculator_v1_0.write(0x4,0x2)
   overlay.ZynqRPNCalculator_v1_0.write(0x4,0x8)
   assert(0x123+0x456 == overlay.ZynqRPNCalculator_v1_0.read(0x8))
   print("PASS")


if __name__ == "__main__":
   overlay = Overlay(IMAGE)
   sanity_check(overlay)
   print("Base Address 0x%x" % overlay.ZynqRPNCalculator_v1_0.mmio.base_addr)

