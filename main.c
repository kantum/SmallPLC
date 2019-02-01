/**
 * @file  main.c
 * @brief Main function of the firmware
 *
 */

/**
 * @brief Entry point
 *
 */
#include <stdint.h>
#include "types.h"

#define PM_ADDR			((u32)0x40000400)
#define SYSCTRL_ADDR	((u32)0x40000800)
#define PORTA_ADDR		((u32)0x41004400)
#define PORTB_ADDR		((u32)0x41004480)
#define PORTB			((u32)0x41004480)
#define GCLK_ADDR		((u32)0x30000c00)
#define UART_ADDR		((u32)0x42000800)

/* PORT Registers */
#define P_DIR			0x00
#define P_DIRCLR		0x04
#define P_DIRSET		0x08
#define P_DIRTGL		0x0c
#define P_OUT			0x10
#define P_OUTCLR		0x14
#define P_OUTSET		0x18
#define P_OUTTGL		0x1c
#define P_IN			0x20
#define P_CTRL			0x24
#define P_WRCONFIG		0x28
#define P_PMUX0			0x30
#define P_PMUX15		0x3f
#define P_PINCFG0		0x40
#define P_PINCFG31		0x5f

/* Power Manager Registers */
#define PM_CTRL			0x00
#define PM_SLEEP		0x01
#define PM_CPUSEL		0x08
#define PM_APBASEL		0x09
#define PM_APBBSEL		0x0A
#define PM_APBCSEL		0x0B
#define PM_AHBMASK		0x14
#define PM_APBAMASK		0x18
#define PM_APBBMASK		0x1c
#define PM_APBCMASK		0x20
#define PM_INTENCLR		0x34
#define PM_INTENSET		0x35
#define PM_INTFLAG		0x36
#define PM_RCAUSE		0x38

/* System Controler Registers */
#define SYSCTRL_INTENCLR	0x00
#define SYSCTRL_INTENSET	0x04
#define SYSCTRL_INTFLAG		0x08
#define SYSCTRL_PCLKSR		0x0c
#define SYSCTRL_XOSC		0x10
#define SYSCTRL_XOSC32K		0x14
#define SYSCTRL_OSC32K		0x18
#define SYSCTRL_OSCULP32K	0x1c
#define SYSCTRL_OSC8M		0x20
#define SYSCTRL_DFLLCTRL	0x24

/* Generic Clock Controller Registers */
#define	GCLK_CTRL			0x00
#define	GCLK_STATUS			0x01
#define	GCLK_CLKCTRL		0x02
#define	GCLK_GENCTRL		0x04
#define	GCLK_GENDIV			0x08

/* SERCOM USART */
#define CTRLA				0x00
#define CTRLB				0x04
#define BAUD				0x0c
#define RXPL				0x0e
#define INTENCLR			0x14
#define INTENSET			0x16
#define INTFLAG				0x18
#define STATUS				0x1A
#define SYNCBUSY			0x1c
#define DATA				0x28
#define DBGCTRL				0x30
#define FIFOSPACE			0x34

#define UART_BAUD    9600
#define UART_GCLK 8000000
#define CONF_BAUD_RATE  (65536 - ((65536 * 16.0f * UART_BAUD) / UART_GCLK))

#define UART_GCLK 8000000

static inline u32 reg_rd(u32 reg)
{
	return( *(volatile u32 *)reg );
}

static inline void reg8_wr(u32 reg, u8 value)
{
	*(volatile u8 *)reg = value;
}

static inline void reg16_wr (u32 reg, u16 value)
{
	*(volatile u16 *)reg = value;
}

static inline void reg_wr(u32 reg, u32 value)
{
	*(volatile u32 *)reg = value;
}

static inline void reg_set(u32 reg, u32 value)
{
  *(volatile u32 *)reg = (*(volatile u32 *)reg | value);
}
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void uart_putc(unsigned char c)
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
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void	init_clock(void)
{
	u32	v;

	/* Configure clock source */
	reg8_wr(PM_ADDR + PM_CPUSEL, 0x00); /* CPU clock select (CPUSEL)  */ 

	/* Configure internal 8MHz oscillator */
	v = reg_rd(SYSCTRL_ADDR + SYSCTRL_OSC8M);
	v &= 0xFFFFFC3F;                 /* Clear prescaler and OnDemand flag */
	reg_wr(SYSCTRL_ADDR + SYSCTRL_OSC8M, v);

	/* Wait for internal 8MHz oscillator stable and ready */
	while( ! (reg_rd(SYSCTRL_ADDR + SYSCTRL_PCLKSR) & 0x08))
		;
	/* Set Divisor GCLK0 : enabled, OSC8M, no divisor */
	reg_wr(GCLK_ADDR + GCLK_GENDIV, (1 << 8) | 0x00);
	reg_wr(GCLK_ADDR + GCLK_GENCTRL, (1 << 16) /* Generic Clock Generator Enable */
			| (0x06 << 8)					   /* Source Select -> OSC8M */
			| 0x00);

	/* Configure PA14 */
	reg_set(PORTA_ADDR + P_DIR, 1 << 14);
	reg_set(PORTA_ADDR + P_OUT, 1 << 14);
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

int main(void)
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
