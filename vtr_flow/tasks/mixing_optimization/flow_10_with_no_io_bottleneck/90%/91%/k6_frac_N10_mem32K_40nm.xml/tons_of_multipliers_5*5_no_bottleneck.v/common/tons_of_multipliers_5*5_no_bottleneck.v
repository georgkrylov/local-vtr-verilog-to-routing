module top_module
(
    
    input [4:0] a1,a2,a3,a4,a5,a6,a7,a8,a9,b1,b2,b3,b4,b5,b6,b7,b8,b9,a11,a12,a13,a14,a15,a16,a17,a18,a19,b11,b12,b13,b14,b15,b16,b17,b18,b19,a10,b10,a20,b20,
    output [9:0] c1,c2,c3,c4,c5, c6,c7,c8,c9,c10
   
);
  assign c1 = a1 * b1;
  assign c2 = a2 * b2;
  assign c3 = a3 * b3;
  assign c4 = a4 * b4;
  assign c5 = a5 * b5;
  assign c6 = a1 * b2;
  assign c7 = a2 * b3;
  assign c8 = a3 * b4;
  assign c9 = a4 * b5; 
  assign c10 = a4 * b1;   
endmodule
