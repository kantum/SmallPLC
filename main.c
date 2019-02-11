#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"
#include "spi.h"

/**
 * @brief Initialize status leds
 * @section Led handling
 */
void	leds_init()
{
	/* Forget bootloader configuration for PB10 */
	reg8_wr((PORTB_ADDR + P_PINCFG + 10), 0);
	/* Configure PB10 and PB11 as output (DIR) */
	reg_wr(PORTB_ADDR + P_DIRSET, 1 << 10 | 1 << 11);
	/* Turn them off*/
	reg_wr(PORTB_ADDR + P_OUTCLR, 1 << 10 | 1 << 11);
}

/**
 * @brief Don't you know main function ? Very handy one...
 */
int		main(void)
{
	unsigned char	str[100];
	unsigned int i;
	leds_init();
	clock_init();
	uart_init(0);
	spi_init(5);
	while (1)
	{
		spi_putc('a');
		uart_puts("DATA register: ");
		uart_puthex(reg_rd(SPI_ADDR + DATA));
		uart_crlf();
		uart_gets(str);
		uart_puts(str);
		uart_crlf();
	}
}
/* EOF */
