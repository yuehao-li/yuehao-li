module modulo_is_zero #(
    parameter WIDTH=16
)
(
    input              clk,
    input              rst_n,
    // INPUT CONTROL
    input              valid_i,
    output             ready_i,
    // INPUT DATA
    input  [WIDTH-1:0] a,
    input  [WIDTH-1:0] b,
    output             y,
    // OUTPUT CONTROL
    output             valid_o,
    input              ready_o
);

localparam COUNTER_WIDTH = $clog2(WIDTH+2);

// DATAPATH
// divisend MOD divisor
reg [WIDTH-1:0] dividend;
reg [(2*WIDTH-1)-1:0] divisor;

// CONTROL
reg busy;
reg [COUNTER_WIDTH-1:0] counter;

wire compute_done = (busy) && ((counter == 0) || (dividend == 0));
wire is_zero = (dividend == 0);

always @(posedge clk) begin
    if (~rst_n)
        busy <= 1'b0;
    else if (valid_i && ready_i)
        busy <= 1'b1;
    else if (valid_o && ready_o)
        busy <= 1'b0;
    else
        busy <= busy;
end

always @(posedge clk) begin
    if (valid_i && ready_i)
        counter <= WIDTH;
    else if (busy)
        counter <= counter - 1;
    else
        counter <= counter;
end

always @(posedge clk) begin
    if (valid_i && ready_i)
        divisor <= b << (WIDTH-1);
    else if (busy)
        divisor <= divisor >> 1;
    else
        divisor <= divisor;
end

always @(posedge clk) begin
    if (valid_i && ready_i)
        dividend <= a;
    else if (busy)
        dividend <= (dividend < divisor) ? dividend : (dividend - divisor);
    else
        dividend <= dividend;
end

assign ready_i = ~busy;
assign valid_o = compute_done;
assign y = is_zero;

endmodule