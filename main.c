#include <stdint.h>
#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"

/**
 * @brief Initialize status leds
 * @section Led handling
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

u8		uart_gets(u8 *c)
{
	int		i;
	char	*str;

	i = -1;
	while (reg16_rd(UART_ADDR + INTFLAG & 0x02) == 0) /* RXC Receive Complete */
		; /* TODO */
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 10);
	//while (reg16_rd(UART_ADDR + FIFOSPACE & 0xff00) != 0) /* RXSPACE */
	str[++i] = reg8_rd(UART_ADDR + DATA);
	str[++i] = '\0';
}

int		main(void)
{
	unsigned char	*str;
	unsigned int i;
	uart_init(0);

	leds_init();

	i = 3 + 1;
	while (--i > 0)
	{
		uart_puts("Aie");
		uart_crlf();
	}
	while (1)
	{
		uart_gets(str);
		//reg_wr(PORTB_ADDR + P_OUTSET, 1 << 11);
		uart_puts(str);
	}
}
/* EOF */
