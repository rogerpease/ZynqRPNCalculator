//
// Roger D. Pease (C) 2024
// Implement a simple Zynq RPN Calculator. 
//  https://rogerpease.com/index.php/zynq-7000-rpn-calculator-with-kernel-driver/ for more details. 
// 
`timescale 1 ns / 1 ps

module ZynqRPNCalculator #(

   parameter integer STACKDEPTH = 32

)
(
    input  [31:0] value,
    input  wire   clock,
    input  wire   reset,
    input  wire   pop,
    input  wire   push,
    input  wire   add,
    input  wire   sub,
    input  wire   mul, 
    output [31:0] stack0
);


    reg [STACKDEPTH-1:0][31:0] stack;
    integer stack_index;

    //
    // Manage the stack. Either pop something on, push something off, or perform an operation. 
    //
    always @(posedge clock)
    begin
     
      if (reset)
        begin
          for (stack_index = 0; stack_index < STACKDEPTH;stack_index = stack_index + 1)
             stack[stack_index] = 0;
        end 
      else if (push) 
        begin 
          for (stack_index = STACKDEPTH-2; stack_index >= 0;stack_index = stack_index - 1)
            stack[stack_index+1] = stack[stack_index];
          stack[0] = value;
        end
      else if (pop) 
        begin 
          for (stack_index = 0; stack_index < STACKDEPTH-1;stack_index = stack_index + 1)
            stack[stack_index] = stack[stack_index +1];
        end 
      else if (add || sub || mul) 
        begin 

          if      (add) stack[0] = stack[0]+stack[1]; 
          else if (sub) stack[0] = stack[1]-stack[0]; 
          else if (mul) stack[0] = stack[1][7:0]*stack[0][7:0]; 

          for (stack_index = 1; stack_index < STACKDEPTH-1;stack_index = stack_index + 1)
            stack[stack_index] = stack[stack_index+1];

        end 

    end 

    assign stack0 = stack[0];

endmodule
