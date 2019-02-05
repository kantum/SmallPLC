#include <stdint.h>
#include "types.h"
#include "hardware.h"
#include "samd21.h"

#define UART_BAUD    	9600
#define UART_GCLK    	8000000
#define CONF_BAUD_RATE	(65536 - ((65536 * 16.0f * UART_BAUD) / UART_GCLK))

void	uart_putc(unsigned char c)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ( (reg_rd(UART_ADDR + 0x18) & 0x01) == 0)
		;
	/* Write data */
	reg_wr((UART_ADDR + 0x28), c);
}

void uart_puts(char *s)
{
	/* Loop to process each character */
	while(*s)
	{
		/* Send one byte */
		uart_putc(*s);
		/* Move pointer to next byte */
		s++;
	}
}

void uart_crlf(void)
{
	uart_puts("\r\n");
}

void	init_sercom(u8 n, u8 clk)
{
	reg_set(PM_ADDR + PM_APBCMASK, 1 << (n + 2));
	reg16_wr (GCLK_ADDR + GCLK_CLKCTRL, (1 << 14) /* Clock enable */
			| (clk << 8)                          /* GCLKGEN[clk] */
			| 20 + n);                            /* SERCOM[n]_CORE */
}

void	init_uart(u8 sercom)
{
	/* Configure Pins */
	reg8_wr((PORTA_ADDR + P_PINCFG + 8), 0x01);   /* PA08 */
	reg8_wr((PORTA_ADDR + P_PINCFG + 9), 0x01);   /* PA09 */
	/* Multiplexer for function C*/
	reg8_wr((PORTA_ADDR + P_PMUX + 4), (0x02) | (0x02 << 4));
	/* Set Divisor GCLK0 : enabled, OSC8M, no divisor */
	reg_wr(GCLK_ADDR + GCLK_GENDIV, (1 << 8) | 0x00);
	/* Generic Clock Generator Enable */
	reg_wr(GCLK_ADDR + GCLK_GENCTRL, (1 << 16)    /* Enable */
			| (0x06 << 8)					      /* Source Select -> OSC8M */
			| 0x01);                              /* Select GCLK1 */
	init_sercom(sercom, 1);

	/* Reset UART (set SWRST) */
	reg_wr((UART_ADDR + CTRLA), 0x01);
	/* Wait end of software reset */
	while(reg_rd(UART_ADDR + CTRLA) & 0x01)
		;
	reg_wr(UART_ADDR + CTRLA, 0x40000000          /* DORD LSB first */
			| 0x100000                            /* RXPO PAD[1] TXPO PAD[0] */
			| 0x04);                              /* Internal Clock */
	/* Enable TX and RX */
	reg_wr(UART_ADDR + CTRLB, 0x00030000);

	/* Configure Baudrate */
	reg16_wr(UART_ADDR + BAUD, CONF_BAUD_RATE);

	reg_set(UART_ADDR, 0x02);

	reg_wr(PORTA_ADDR + P_DIR, 1 << 4);
	reg_wr(PORTA_ADDR + P_OUTCLR, 1 << 4);

}

int		main(void)
{
	/* Configure Pin */
	reg8_wr((PORTB_ADDR + P_PINCFG + 10), 0);
	/* Configure PB10 (DIR) */
	reg_wr(PORTB_ADDR + P_DIR, 1<<10);
	/* Set PB10 value (OUT) */
	reg_wr(PORTB_ADDR + P_OUTCLR, 1<<10);
	/* Configure PB11 (DIR) */
	reg_set(PORTB_ADDR + P_DIR, 1<<11);
	/* Set PB11 value (OUT) */
	reg_set(PORTB_ADDR + P_OUTCLR, 1<<11);

	init_uart(0);
	while (1)
	{
		uart_puts("bieres !");
		uart_crlf();
	}
}

/* EOF */
