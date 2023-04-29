`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/04/27 15:27:47
// Design Name: 
// Module Name: fifo
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module fifo #(parameter WIDTH=64, parameter DEPTH=8)(
    input clk, 
    input res_n, 
    input shift_in, 
    input shift_out, 
    input [WIDTH-1:0] data_in,  
    output full, 
    output empty,
    output [WIDTH-1:0] data_out
    );
    wire [DEPTH-1:0] valid;
    wire [WIDTH-1:0] out [DEPTH-1:0];
    assign full = &valid;
    assign empty = ~(|valid);
    assign data_out = out[DEPTH-1] ;
    genvar i;
    generate
        for (i = 0; i < DEPTH; i = i+1) begin
            if (i == 0)begin
            // First FF
                FF ff(  .clk(clk), .res_n(res_n), .si(shift_in), .so(shift_out), 
                        .data_in(data_in), .pre(1'b0), .next(valid[i+1]), .pre_out(0), 
                        .out(out[i]), .valid(valid[i]));
            end else if (i == DEPTH - 1) begin
            // Last FF
                FF ff(  .clk(clk), .res_n(res_n), .si(shift_in), .so(shift_out), 
                        .data_in(data_in), .pre(valid[i-1]), .next(1'b1), .pre_out(out[i-1]), 
                        .out(out[i]), .valid(valid[i]));
            end else begin
            // Middle FF
                FF ff(  .clk(clk), .res_n(res_n), .si(shift_in), .so(shift_out), 
                        .data_in(data_in), .pre(valid[i-1]), .next(valid[i+1]), .pre_out(out[i-1]), 
                        .out(out[i]), .valid(valid[i]));
            end
        end
    endgenerate
endmodule

module FF #(parameter WIDTH = 64, parameter DEPTH=8)(
    input clk,
    input res_n,
    input [WIDTH - 1: 0] data_in,
    input [WIDTH - 1: 0] pre_out,
    input si,
    input so,
    input pre,
    input next,
    output [WIDTH - 1:0] out,
    output reg valid
);
    reg [WIDTH-1:0] data;
    assign out = data;
    always @(posedge clk or negedge res_n) begin
        if (~res_n) begin
            data <= 0;
            valid <= 1'b0;
        end else begin
            if (~so && ~si) begin
                // Both Low
                data <= data;
                valid <= valid;
            end else if (so && si) begin
                if(valid && ~pre) begin
                // shift in new data in 
                    data <= data_in;
                    valid <= 1'b1;
                end else begin
                // shift in pre
                    data <= pre_out;
                    valid <= pre;
                end
            end else if (~so && si ) begin
                if (~valid && next) begin
                    // shift in current stage
                    data <= data_in;
                    valid <= 1'b1;
                end else begin
                    // hold value 
                    data <= data;
                    valid <= valid;
                end
            end else if (so && ~si) begin
                data <= pre_out;
                valid <= pre;
            end else begin
                // Wrong stage
                data <= data;
                valid <= 1'bx;
            end
            
        end
    end
endmodule 
