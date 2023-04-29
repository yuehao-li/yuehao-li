`timescale 1ns / 1ps

module directed_test;



    reg clk, res_n, shift_in, shift_out;
    reg [63:0] data_in;
    
    wire  full, empty;
    wire [63:0] data_out;
    
    // Instantiate the fifo
    fifo dut(.clk(clk), 
              .res_n(res_n), 
              .shift_in(shift_in),
              .shift_out(shift_out), 
              .data_in(data_in),
              .full(full),
              .empty(empty),
              .data_out(data_out));
              
              

    always 
        #5 clk = !clk;
         
        
    initial begin
    
    
         // Initialize inputs and reset
        clk = 0;
        res_n = 0;
        
        @(posedge clk); #1 res_n = 1;
        data_in = 64'b1;
        
            shift_in = 0;
            shift_out = 0;
        
        //shift in 8 times
        repeat (8) begin
            @(posedge clk);
            #1;
            shift_in = 1;
            shift_out = 0;
            data_in = data_in + 1;
            
        end 
        
        repeat (8) begin
            @(posedge clk);
            #1;
            shift_in = 0;
            shift_out = 1;
        end 
        
    end
endmodule
        