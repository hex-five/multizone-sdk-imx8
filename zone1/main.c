/* Copyright(C) 2020 Hex Five Security, Inc. - All Rights Reserved */

#include <fcntl.h>	// open()
#include <unistd.h> // read() write()
#include <string.h>	// strxxx()
#include <stdio.h>	// printf() sprintf()
#include <stdlib.h> // qsort() strtoul()
#include <limits.h> // UINT_MAX ULONG_MAX
#include <math.h> // round()

#include <unistd.h>

#include <platform.h>
#include "multizone.h"
#include "gpio_driver.h"

#define BUFFER_SIZE 16
static struct{
	char data[BUFFER_SIZE];
	volatile int p0; // read
	volatile int p1; // write
} buffer;

static char inputline[32+1]="";


#define CMD_LINE_SIZE 32
#define MSG_SIZE 4



void uart_handler(void) __attribute__ ((interrupt ("irq"))); 
void uart_handler(void){

	UART_IRQ_CLR();
	if(UART_IRQ_RX()) {
		if (buffer.p0==buffer.p1) {buffer.p0=0; buffer.p1=0;}
		read(0, &buffer.data[buffer.p1++], 1);
		if (buffer.p1 > BUFFER_SIZE-1) buffer.p1 = BUFFER_SIZE-1;
	}
}


void print_cpu_info(void) {


	/* SCB->CPUID Special register access (read-only) */
	//uint32_t cpuid = LOAD_SCB(SCS_SCB_CPUID);			// mzone api
	//uint32_t cpuid = *(volatile uint32_t *)SCB_CPUID;	// trap&emul
	uint32_t cpuid = 0x410FC241;

	/* Decoding SCB->CPUID */
	uint16_t partno = (cpuid&0xfff0)>>4;

	const char *partno_str = (partno==0xC23 ? "Cortex-M3.\n\0" : \
				  partno==0xC24 ? "Cortex-M4.\n\0" : \
				  partno==0xC27 ? "Cortex-M7.\n\0" : "Undefined.\n\0");

	printf("Implementer    : 0x%X, Arm.\n", (cpuid >> 24));
	printf("Variant        : 0x%X, Revision %d.\n", ((cpuid&0x00ffffff)>>20),((cpuid&0x00ffffff)>>20));
	printf("PartNo         : 0x%X, %s", partno, partno_str);
	printf("Revision       : 0x%X, Patch %d.\n", ((cpuid&0x7)),((cpuid&0x7)));

}


int cmpfunc(const void* a , const void* b){

    const int ai = *(const int* )a;
    const int bi = *(const int* )b;
    return ai < bi ? -1 : ai > bi ? 1 : 0;
}


void msg_handler() {

	// Message handler
	for (int zone=2; zone<=4; zone++){

		char msg[16];

		if (MZONE_RECV(zone, msg)) {

			if (strcmp("ping", msg) == 0)
				MZONE_SEND(zone, "pong");

			else {
				write(1, "\e7\e[2K", 6);   // save curs pos & clear entire line
				printf("\rZ%d > %.16s\n", zone, msg);
				write(1, "\nZ1 > %s", 6); write(1, inputline, strlen(inputline));
				write(1, "\e8\e[2B", 6);   // restore curs pos & curs down 2x
			}
		}

	}

}


void cmd_handler(){

}


char readline(){

	return 0;

}




// ------------------------------------------------------------------------
int main (void) {

	//volatile int w=0; while(1){w++;}
	//while(1) MZONE_YIELD();
	//while(1) MZONE_WFI();

	/* Disable Watchdog */
	//uint32_t * cm4_wdog_cnt = 0xAAAA

	/* Open IPC Channel */
	/* Clear GIEn, RIEn, TIEn, GIRn and ABFn. */ 
	uint32_t * mu_acr = (uint32_t *) 0x41480024;
	*mu_acr = 0x0;

	/* Configures pin routing */
	uint32_t * mu_atr0 = (uint32_t *) 0x41480000;
	uint32_t * mu_atr1 = (uint32_t *) 0x41480004;
	uint32_t * mu_atr2 = (uint32_t *) 0x41480008;
	uint32_t * mu_atr3 = (uint32_t *) 0x4148000C;
	/* BOARD_INITPINS_FTDI_M40_UART0_RX_PIN_FUNCTION_ID */
	*mu_atr0 = 0x05060401;
	*mu_atr1 = 0x0;
	*mu_atr2 = 0x10007;
	*mu_atr3 = 0x0;
	/* Spend some time */
	for(volatile int i=0; i<100; i++);
	/* Pad 2 */
	*mu_atr0 = 0x05060401;
	*mu_atr1 = 0x0;
	*mu_atr2 = 0x10008;
	*mu_atr3 = 0x0;


	/* */
	*mu_atr0 = 0x05020301;
	*mu_atr1 = 0x7f28155;
	*mu_atr2 = 0x2011f;

	open("UART", 0, 0);
	//STORE_NVICISER(UART_IRQn);

	printf("\e[2J\e[H"); // clear terminal screen
	printf("=====================================================================\n");
	printf("      	             Hex Five MultiZone® Security                    \n");
	printf("    Copyright© 2020 Hex Five Security, Inc. - All Rights Reserved    \n");
	printf("=====================================================================\n");
	printf("This version of MultiZone® Security is meant for evaluation purposes \n");
	printf("only. As such, use of this software is governed by the Evaluation    \n");
	printf("License. There may be other functional limitations as described in   \n");
	printf("the evaluation SDK documentation. The commercial version of the      \n");
	printf("software does not have these restrictions.                           \n");
	printf("=====================================================================\n");

    print_cpu_info();
	//uint32_t cpuid = LOAD_SCB(SCS_SCB_CPUID);

	write(1, "\n\rZ1 > ", 7);

	while(1);

    while(1){

		if (readline()){
			cmd_handler(); //printf("%s\n", inputline);
			write(1, "\n\rZ1 > ", 7);
			inputline[0]='\0';
		}

		msg_handler();

		//MZONE_YIELD();
		MZONE_WFI();

	}

} // main()
