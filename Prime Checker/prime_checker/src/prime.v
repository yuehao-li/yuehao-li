`timescale 1ns / 1ps
module is_prime
#(
    parameter WIDTH = 16
) (
    input clk,
    input rst_n,

    input valid_i,
    output ready_i,

    input [WIDTH-1:0] number,
    output reg result,
    
    output valid_o,
    input ready_o
);
    parameter IDLE=3'b000, PRE_CHECK = 3'b001, START = 3'b010, WAIT = 3'b011, DONE = 3'b100, FAIL= 3'b101;
    reg [2:0] state, next_state;
    reg [WIDTH-1:0] testnum;
    reg plusone;
    wire sub_valid_i, sub_ready_i;
    wire sub_valid_o, sub_ready_o;
    wire modulo_result;
    assign valid_o = state==DONE;
    assign ready_i = state==IDLE;
    assign sub_valid_i = state == START;
    assign sub_ready_o = 1;
     modulo_is_zero m1(.clk(clk), .rst_n(rst_n), .a(number), .b(testnum), .y(modulo_result), .valid_o(sub_valid_o), .valid_i(sub_valid_i), .ready_i(sub_ready_i), .ready_o(sub_ready_o));
    always @(*) begin
        if(!rst_n)begin
            // 
        end else begin
            casex(state) 
            IDLE: begin
                testnum <= 2;
            end
            PRE_CHECK: begin
                if(number == 2)
                    result <= 1;
                else 
                    result <= 0;
            end
            START: begin

            end
            WAIT: begin
                // isPrime = True
                if((testnum > (number>>1)) && (sub_valid_o == 1) && (modulo_result == 0)) begin
                    plusone <= 0;
                    //testnum <= testnum;
                    result <= 1;
                end
                // isPrime = False ()
                else if ((modulo_result == 1) && (sub_valid_o == 1)) begin
                    plusone <= 0;
                    result <= 0;
                    //testnum <= testnum;
                end
                // Not Sure, Go back to Wait State, Testnumber + 1
                else if ((sub_valid_o == 1) && (modulo_result == 0) && ( (number>>1) >= testnum) ) begin
                    plusone <= 1;
                    //testnum <= testnum + 1;
                    result <= 0;
                end
                else begin  
                    plusone <=0;
                    //testnum <= testnum;
                    result <= 0;
                end
            end
            DONE: begin

            end

            endcase
        end
    end



// State Machine States
    always @(*) begin
        casex(state)
            IDLE:       begin
                            if(valid_i) begin
                                next_state <= PRE_CHECK;
                            end else begin
                                next_state <= IDLE;
                            end
                        end
            PRE_CHECK:  begin
                            if(number == 1 || number == 2) begin
                                next_state <= DONE;
                            end else begin
                                next_state <= START;
                            end
                        end
            START:      begin
                            next_state <= WAIT;
                        end
            WAIT:       begin
                            // 
                            if ((modulo_result == 1 && sub_valid_o == 1 ) || (testnum > (number>>1)  && sub_valid_o == 1 && modulo_result == 0)) begin
                                next_state <= DONE;
                            
                            end else if (!modulo_result && sub_valid_o == 1 && (  (number>>1) >= testnum)) begin
                                // go back START
                                next_state <= START;
                            end else begin
                                // stay in current 
                                next_state <= WAIT;
                            end
                        end
            DONE:       begin
                            if (ready_o) begin
                                next_state <= IDLE;
                            end else begin
                                // Stay in current state
                                next_state <= DONE;
                            end
                        end
            default:    begin
                        
                        
                        end
        endcase
    end
    
// State transition at clk edge 
    always @(posedge clk, negedge rst_n) begin
        if (!rst_n) begin
        // Reset
            state <= IDLE;
        end else begin
            state <= next_state;
            if(plusone)
                testnum <= testnum + 1;
            else
                testnum <= testnum;
        end
    end


    

endmodule

