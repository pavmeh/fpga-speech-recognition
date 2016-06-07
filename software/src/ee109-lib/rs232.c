#include "system.h"
#include "rs232.h"

char get_char(void) {
volatile int* RS232_UART_ptr = (int*) SERIAL_PORT_BASE; // RS232 UART address
int data;
data = *(RS232_UART_ptr); // read the RS232_UART data register
if (data & 0x00008000) // check RVALID to see if there is new data
	return((char) data & 0xFF);
else
	return('\0');
}

unsigned char put_char(char c) {
	volatile int* RS232_UART_ptr = (int *) SERIAL_PORT_BASE; // RS232 UART address
	int control;
	control = *(RS232_UART_ptr + 1);   // read the RS232_UART control register
	if (control & 0x00FF0000) {
		*(RS232_UART_ptr) = c;
		//success
		return (unsigned char) 0;
	}
	return (unsigned char) 1;
}
