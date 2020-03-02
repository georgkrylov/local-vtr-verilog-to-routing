`define WIDTH 2
`define MULT_WIDTH (`WIDTH * 2)

module top_module
(
    
    input [`WIDTH-1:0] a1,b1,
    output [`MULT_WIDTH-1:0] c1
   
);

  assign c1 = a1*b1;

endmodule
