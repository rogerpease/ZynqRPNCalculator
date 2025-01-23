#!/bin/sh 

set -e 
verilator ../../verilog/src/ZynqRPNCalculator.v --cc  --exe TestModule.cpp ZynqRPNCalculatorVerification.cpp --build

exit 0
