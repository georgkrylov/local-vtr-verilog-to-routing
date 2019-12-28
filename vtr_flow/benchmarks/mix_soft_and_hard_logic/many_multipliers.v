module top_module
(
    
    input a1,a2,b1,b2,a3,b3,a4,b4,
    output [1:0] c1,c4,
    output [3:0] c2,c3
   
);
  assign c4 = a4*b4;
  assign c1 = a1*b1;
  assign c2 = c1*b2;
  assign c3 = c1*a2; 
endmodule
