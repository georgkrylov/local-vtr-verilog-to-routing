module top_module
(
    
    input [13:0] a1,a2,a3,b1,b2,b3,
    output [27:0] c1,c2,c3,c4,c5
   
);
  assign c1 = a1 * b1;
  assign c2 = a2 * b2;
  assign c3 = a3 * b3;
  assign c4 = a2 * b1;
  assign c5 = a2 * b3;

endmodule
