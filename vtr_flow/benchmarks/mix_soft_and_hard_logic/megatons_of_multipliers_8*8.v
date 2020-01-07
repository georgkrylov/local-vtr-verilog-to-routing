module top_module
(
    
    input [7:0] a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5,b6,b7,b8,b9,
    output [15:0] c1,c2,c3,c4,c5,c6,c7,c8,c9
   
);
  assign c1 = a1 * b1;
  assign c2 = a2 * b2;
  assign c3 = a3 * b3;
  assign c4 = a4 * b4;
  assign c5 = a5 * b5;
  assign c6 = a6 * b6;
  assign c7 = a7 * b7;
  assign c8 = a8 * b8;
  assign c9 = a9 * b9;

endmodule
