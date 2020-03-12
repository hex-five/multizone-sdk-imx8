/* Copyright(C) 2018 Hex Five Security, Inc. - All Rights Reserved */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include <platform.h>

// ----------------------------------------------------------------------------
int _close(int file) {
// ----------------------------------------------------------------------------

	return -1;
}

// ----------------------------------------------------------------------------
int _fstat(int file, struct stat *st) {
// ----------------------------------------------------------------------------

	st->st_mode = S_IFCHR;
	return 0;
}

// ----------------------------------------------------------------------------
void * _sbrk(int incr) {
// ----------------------------------------------------------------------------

	extern char _end[];
	extern char _heap_end[];
	static char *_heap_ptr = _end;

	if ((_heap_ptr + incr < _end) || (_heap_ptr + incr > _heap_end))
		return  (void *) -1;

	_heap_ptr += incr;
	return _heap_ptr - incr;
}

// ----------------------------------------------------------------------------
int _isatty(int file) {
// ----------------------------------------------------------------------------

	return (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) ? 1 : 0;

}

// ----------------------------------------------------------------------------
int _lseek(int file, off_t ptr, int dir) {
// ----------------------------------------------------------------------------

	return 0;
}

// ----------------------------------------------------------------------------
int _open(const char* name, int flags, int mode) {
// ----------------------------------------------------------------------------

	if (strcmp(name, "UART")==0){

		/* Check UART ID */
		uint32_t lpuart_verid = LPUART_REG(LPUART_VERID);
		uint32_t temp = 0;

		uint32_t lpuart_param = 0xdeaddead;
		uint32_t lpuart_global = 0xdeaddead;
		uint32_t lpuart_pincfg = 0xdeaddead;
		uint32_t lpuart_baud = 0xdeaddead;
		uint32_t lpuart_stat = 0xdeaddead;
		uint32_t lpuart_ctrl = 0xdeaddead;
		uint32_t lpuart_data = 0xdeaddead;
		uint32_t lpuart_match = 0xdeaddead;
		uint32_t lpuart_modir = 0xdeaddead;
		uint32_t lpuart_fifo = 0xdeaddead;
		uint32_t lpuart_water = 0xdeaddead;

		if(lpuart_verid != LPUART_VERID_VAL) {
			return -1;
		}

		/* Resets LPUART */
		LPUART_REG(LPUART_GLOBAL) |= LPUART_GLOBAL_RST;
		LPUART_REG(LPUART_GLOBAL) &= ~LPUART_GLOBAL_RST;

		/* 
		* configure USART baud rate value 
		*/
		LPUART_REG(LPUART_BAUD) = 0x50200bf; // TBD
		//LPUART_REG(LPUART_BAUD) &= ~LPUART_BAUD_SBNS_M10;

		/* Reset CTRL */
		LPUART_REG(LPUART_CTRL) = 0;


		/**/
		LPUART_REG(LPUART_WATER) = 0;

		/* Enable TX/RX FIFO */
		LPUART_REG(LPUART_FIFO) |= (LPUART_CTRL_TXFE | LPUART_CTRL_RXFE);

		/* Flush FIFO */
		LPUART_REG(LPUART_FIFO) |= (LPUART_CTRL_TXFLUSH | LPUART_CTRL_RXFLUSH);

		/* Clear all status flags */
		LPUART_REG(LPUART_STAT) |= 0xc01fc000;

		/* configure USART receiver */
		//USART0_REG(USART_CTL0) &= ~(USART_CTL0_REN); 
		//USART0_REG(USART_CTL0) |= (USART_CTL0_REN);

		/* configure USART transmitter */
		//USART0_REG(USART_CTL0) &= ~(USART_CTL0_TEN); 
		//USART0_REG(USART_CTL0) |= (USART_CTL0_TEN);

		temp = LPUART_REG(LPUART_CTRL);
		/* enable USART */
		temp |= LPUART_CTRL_TE_E;
		temp |= LPUART_CTRL_RE_E;
		LPUART_REG(LPUART_CTRL) = temp;


		/* enable USART interrupt */
		//USART0_REG(USART_CTL0) |= (USART_CTL0_RBNEIE);

		lpuart_param = LPUART_REG(LPUART_PARAM);
		lpuart_global = LPUART_REG(LPUART_GLOBAL);
		lpuart_pincfg = LPUART_REG(LPUART_PINCFG);
		lpuart_baud = LPUART_REG(LPUART_BAUD);
		lpuart_stat = LPUART_REG(LPUART_STAT);
		lpuart_ctrl = LPUART_REG(LPUART_CTRL);
		lpuart_data = LPUART_REG(LPUART_DATA);
		lpuart_match = LPUART_REG(LPUART_MATCH);
		lpuart_modir = LPUART_REG(LPUART_MODIR);
		lpuart_fifo = LPUART_REG(LPUART_FIFO);
		lpuart_water = LPUART_REG(LPUART_WATER);

		return 0;

	}

	return -1;
}

// ----------------------------------------------------------------------------
int _read(int file, char *ptr, size_t len) {
// ----------------------------------------------------------------------------
	if (isatty(file)) {

	}
	return -1;
}

// ----------------------------------------------------------------------------
size_t _write(int file, const void *ptr, size_t len) {
// ----------------------------------------------------------------------------

	if (isatty(file)) {

		const uint8_t * buff = (uint8_t *)ptr;

		for (size_t i = 0; i < len; i++) {

			while ((LPUART_REG(LPUART_STAT) & LPUART_STAT_TRDE) == 0){;}

			LPUART_REG(LPUART_DATA) = (char) buff[i];

			if (buff[i] == '\n') {
				while ((LPUART_REG(LPUART_STAT) & LPUART_STAT_TRDE) == 0){;}
				LPUART_REG(LPUART_DATA) = '\r';
			}
		}

	}

	return -1;
}
