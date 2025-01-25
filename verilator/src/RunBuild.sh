#!/bin/sh 

set -e 
verilator ../../verilog/src/ZynqRPNCalculator.v --cc  --exe ZynqRPNCalculatorTestModule.cpp ZynqRPNCalculatorVerification.cpp --build  -Wno-TIMESCALEMOD
exit 0
