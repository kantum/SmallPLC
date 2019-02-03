#include <stdint.h>
#include "types.h"
#include "samd21.h"

#define UART_BAUD    	9600
#define UART_GCLK    	8000000
#define CONF_BAUD_RATE	(65536 - ((65536 * 16.0f * UART_BAUD) / UART_GCLK))

#define UART_GCLK		8000000

static inline u32	reg_rd(u32 reg)
{
	return( *(volatile u32 *)reg );
}

static inline void	reg8_wr(u32 reg, u8 value)
{
	*(volatile u8 *)reg = value;
}

static inline void	reg16_wr(u32 reg, u16 value)
{
	*(volatile u16 *)reg = value;
}

static inline void	reg_wr(u32 reg, u32 value)
{
	*(volatile u32 *)reg = value;
}

static inline void	reg_set(u32 reg, u32 value)
{
  *(volatile u32 *)reg = (*(volatile u32 *)reg | value);
}

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void	uart_putc(unsigned char c)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ( (reg_rd(UART_ADDR + 0x18) & 0x01) == 0)
		;
	/* Write data */
	reg_wr((UART_ADDR + 0x28), c);
}

void	uart_puts(char *s)
{
	while(*s)
	{
		uart_putc(*s);
		s++;
	}
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void	init_clock(void)
{
	u32	v;

	/* Configure clock source */
	reg8_wr(PM_ADDR + PM_CPUSEL, 0x00); /* CPU clock select (CPUSEL)  */ 

	/* Configure internal 8MHz oscillator */
	v = reg_rd(SYSCTRL_ADDR + SYSCTRL_OSC8M);
	v &= 0xFFFFFC3F;                    /* Clear prescaler and OnDemand flag */
	reg_wr(SYSCTRL_ADDR + SYSCTRL_OSC8M, v);

	/* Wait for internal 8MHz oscillator stable and ready */
	while( ! (reg_rd(SYSCTRL_ADDR + SYSCTRL_PCLKSR) & 0x08))
		;
	/* Set Divisor GCLK0 : enabled, OSC8M, no divisor */
	reg_wr(GCLK_ADDR + GCLK_GENDIV, (1 << 8) | 0x00);
	/* Generic Clock Generator Enable */
	reg_wr(GCLK_ADDR + GCLK_GENCTRL, (1 << 16)
			| (0x06 << 8)					   /* Source Select -> OSC8M */
			| 0x00);
}

/**
 * @brief Init of sercom interface
 *
 */
void	sercom_init(u8 n, u8 clk)
{
	reg_set(PM_ADDR + PM_APBCMASK, 1 << (n + 2));
	reg16_wr (GCLK_ADDR + GCLK_CLKCTRL, (1 << 14) /* Clock enable */
			| (clk << 8)                          /* GCLKGEN[clk] */
			| 19 + n);                            /* SERCOM[n]_CORE */
}

void	uart_init(void)
{
	/* Configure Pin to Sercom0*/

	/* PA08 to be controlled */
	reg8_wr((PORTA_ADDR + P_PINCFG0 + 8), 1);
	/* Multiplexer for function C*/
	reg8_wr((PORTA_ADDR + P_PMUX0), (0x2));
	/* PA09 to be controlled */
	reg8_wr((PORTA_ADDR + P_PINCFG0 + 9), 1);
	/* Multiplexer for function C*/
	reg8_wr((PORTA_ADDR + P_PMUX0), (0x2 << 4));
	/* Reset UART (set SWRST) */
	reg_wr((UART_ADDR + CTRLA), 0x01);
	/* Wait end of software reset */
	while( reg_rd(UART_ADDR + CTRLA) & 0x01)
		;

	/* Configure UART */

	/* DORD LSB, RXPO PAD[1], USART with internal clock */
	reg_wr(UART_ADDR + 0x00, 0x40100004);
	/* Enable TX and RX */
	reg_wr(UART_ADDR + 0x04, 0x00030000);

	/* Configure Baudrate */
	reg_wr(UART_ADDR + 0x0C, CONF_BAUD_RATE);
}

int		main(void)
{
    /* Configure Pin */
	reg8_wr((PORTB_ADDR + P_PINCFG0 + 10), 0);
    /* Configure PB10 (DIR) */
	reg_wr(PORTB_ADDR + P_DIR, 1<<10);
    /* Set PB10 value (OUT) */
	reg_wr(PORTB_ADDR + P_OUT, 1<<10);
    /* Configure PB11 (DIR) */
	reg_set(PORTB_ADDR + P_DIR, 1<<11);
    /* Set PB11 value (OUT) */
	reg_set(PORTB_ADDR + P_OUT, 1<<11);
	while (1)
	{
		uart_putc('a');
	}
}
/* EOF */
