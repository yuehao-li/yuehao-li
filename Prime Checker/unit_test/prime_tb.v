`timescale 1ns / 1ps

module prime_tb;

localparam WIDTH = 16;

// DUT signals
reg  clk;
reg  rst_n;

reg  valid_i;
wire ready_i;
reg [WIDTH-1:0] number;
wire result;
wire valid_o;
reg  ready_o;

// Testbench variables
integer fd;
integer num_patterns;
reg correct_result;

is_prime #(
    .WIDTH(WIDTH)
) dut (
    .clk      (clk),
    .rst_n    (rst_n),
    .valid_i  (valid_i),
    .ready_i  (ready_i),
    .number   (number),
    .result   (result),
    .valid_o  (valid_o),
    .ready_o  (ready_o)
);

always #5 clk <= ~clk;

initial begin
    // clock generation
    clk = 0;
    
    // reset sequence
    rst_n = 0;
    valid_i = 1'b0;
    ready_o = 1'b1;           // always ready to read from input
    repeat(2) @(posedge clk);
    rst_n = 1;
    
    // Read test patterns from file
    fd = $fopen("prime_test.txt", "r");
    $fscanf(fd, "%d", num_patterns);
    repeat (num_patterns) begin
        // input transaction
        @(posedge clk);
        $fscanf(fd, "%d %b", number, correct_result);
        @(negedge clk);
        valid_i = 1'b1;
        // wait for ack
        while (~(valid_i && ready_i)) @(negedge clk);
        @(negedge clk);
        valid_i = 1'b0;
        
        // wait for output transaction
        while (~(valid_o && ready_o)) @(negedge clk);
        if (result != correct_result)
            $display("Error: is_prime(%d) = %b ", number, correct_result);
    end
    
    $finish;
end

endmodule