/* Copyright(C) 2019 Hex Five Security, Inc. - All Rights Reserved */

#include <string.h>	// strcmp()
#include <inttypes.h> // uint16_t, ...

#include "platform.h"
#include "multizone.h"
#include "gpio_driver.h"


// ------------------------------------------------------------------------
int main (void) {
// ------------------------------------------------------------------------

	//volatile int w=0; while(1){w++;}
	//while(1) MZONE_YIELD();
	//while(1) MZONE_WFI();

	int i = 0;

	while(1){

		// Message handler
		char msg[16]; if (MZONE_RECV(1, msg)) {
			if (strcmp("ping", msg)==0) MZONE_SEND(1, "pong");
			else if (strcmp("irq=0", msg)==0) __disable_irq();
			else if (strcmp("irq=1", msg)==0) __enable_irq();
			else if (strcmp("block", msg)==0) {volatile int i=0; while(1) i++; }
			else if (strcmp("loop",  msg)==0) {while(1) MZONE_YIELD();}
		}

		// Wait For Interrupt
		MZONE_YIELD();
		//MZONE_WFI();

	}


	return 0;
}
