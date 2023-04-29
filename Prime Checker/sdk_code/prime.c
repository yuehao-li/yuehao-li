#include "xparameters.h"
#include "xstatus.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xuartps_hw.h"
#include "xil_printf.h"
#include "xil_io.h"


#define UART_BASEADDR       XPAR_XUARTPS_0_BASEADDR
#define UART_CLOCK_HZ       XPAR_XUARTPS_0_CLOCK_HZ
#define TEST_BUFFER_SIZE    16

#define CUSTOM_IP_BASEADDR 0x43C00000

u32 read_number();
void send_number(u32);
u32 read_result();
u32 read_counter();

int main() {
	u32 number, result;
	while ((number = read_number())) {
        send_number(number);
    	xil_printf("\n\r> Sent");
    	while (!(result = read_result()));
    	xil_printf("\b\b\b\b");
    	result &= 0x1;
    	if (result)
    		xil_printf("PRIME", result);
    	else
    		xil_printf("NOT PRIME", result);
    	xil_printf(" (#cycles = %u)\n\r", read_counter());
	}
	xil_printf("\n\r  < Quit >\n\r");
    return 0;
}

u32 read_number()
{
	xil_printf("> Number: ");

	u32 digits = 0;
	u32 number = 0;
	u32 Running;
	u8 RecvChar;
	u32 CntrlRegister;

	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);

	/* Enable TX and RX for the device */
	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
			  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
			   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

	Running = TRUE;
	while (Running) {
		// Wait for input
		while (!XUartPs_IsReceiveData(UART_BASEADDR));

		RecvChar = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);

		if (RecvChar >= '0' && RecvChar <= '9') {
			number = 10 * number + RecvChar - '0';
			++digits;
		} else if (RecvChar == '\n' || RecvChar == '\r') {
			Running = FALSE;
		} else if (RecvChar == 127) {
			if (digits > 0) {
				number /= 10;
				--digits;
			} else {
				continue;
			}
		}

		// Echo
		XUartPs_WriteReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET, RecvChar);
	}

	return number;
}

void send_number(u32 number) {
	Xil_Out32(CUSTOM_IP_BASEADDR, number);
}

u32 read_result() {
	return Xil_In32(CUSTOM_IP_BASEADDR + 4);
}

u32 read_counter() {
	return Xil_In32(CUSTOM_IP_BASEADDR + 8);
}
