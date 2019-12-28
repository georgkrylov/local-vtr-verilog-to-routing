module top_module
(
    
    input [4:0] a1,a2,b1,b2,
    output [9:0] c1,c2
   
);
  assign c1 = a1*b1;
  assign c2 = a2*b2;

endmodule
