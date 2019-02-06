#include <stdint.h>
#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"

/**
 * @brief Initialize status leds
 */
void	leds_init()
{
	/* Forget bootloader configuration for PB10 */
	reg8_wr((PORTB_ADDR + P_PINCFG + 10), 0);
	/* Configure PB10 and PB11 as output (DIR) */
	reg_wr(PORTB_ADDR + P_DIR, 1 << 10 | 1 << 11);
	/* Turn them off*/
	reg_wr(PORTB_ADDR + P_OUTCLR, 1 << 10 | 1 << 11);
}

u8		*uart_getc(u8 *c)
{
}

int		main(void)
{
	uart_init(0);
	leds_init();

	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 10);
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 11);
	while (1)
	{
		uart_puts("Write something: ");
		uart_crlf();
	}
}

/* EOF */
