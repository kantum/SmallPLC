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

/**
 * @brief Give the size of string
 * @param String to mesure
 */
u32		strlen(u8 *str)
{
	u32	i;

	i = -1;
	while (str[++i])
		return(i);
}

/**
 * @brief Put the octet from DATA to *str
 * @param str The addresse where to put the data
 */
u8		uart_gets(u8 *str)
{
	int		i;

	i = 0;
	while (1)
	{
		/* Wait for start bit */
		while ((reg8_rd(UART_ADDR + INTFLAG) & (1 << 2)) == 0)
			;
		/* Get the data */
		str[i] = (reg16_rd(UART_ADDR + DATA));
		if (str[i] == 0x0d || str[i] == '\0')
			break;
		i++;
	}
	str[i] = '\0';
	return (strlen(str));
}

int		main(void)
{
	unsigned char	str[100];
	unsigned int i;
	uart_init(0);
	leds_init();
	uart_puts("hello");
	uart_crlf();
	while (1)
	{
		uart_gets(str);
		uart_puts(str);
		uart_crlf();
	}
}
/* EOF */
