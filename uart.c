/**
 * @file  uart.c
 * @brief Handle UART port (using sercom peripheral)
 *
 * @author Saint-Genest Gwenael <gwen@cowlab.fr>
 * @copyright Cowlab (c) 2017
 *
 * @page License
 * CowStick-bootloader is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 3 as published by the Free Software Foundation. You
 * should have received a copy of the GNU Lesser General Public
 * License along with this program, see LICENSE.md file for more details.
 * This program is distributed WITHOUT ANY WARRANTY see README file.
 */
#include "hardware.h"
#include "uart.h"
#include "samd21.h"

static const u8 hex[16] = "0123456789ABCDEF";

/**
 * @brief Send end-of-line string CR-LF over UART
 *
 */
void uart_crlf(void)
{
	uart_puts("\r\n");
}

/**
 * @brief Initialize and configure SERCOM module
 * @param n SERCOM number
 * @param clk Chose wich clock to use GCLKGEN number
 */
void	sercom_init(u8 n, u8 clk)
{
	reg_set(PM_ADDR + PM_APBCMASK, 1 << (n + 2));
	reg16_wr (GCLK_ADDR + GCLK_CLKCTRL, (1 << 14) /* Clock enable */
			| (clk << 8)                          /* GCLKGEN[clk] */
			| 20 + n);                            /* SERCOM[n]_CORE */
}

/**
 * @brief Initialize and configure UART port
 * @param 
 */
void	uart_init(u8 sercom)
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
	sercom_init(sercom, 1);

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

	/* Power up the PMOD Module */
	reg_wr(PORTA_ADDR + P_DIR, 1 << 4);
	reg_wr(PORTA_ADDR + P_OUTCLR, 1 << 4);

	/* Set uart to on */
	reg_set(UART_ADDR, 0x02);
}

//void uart_init(u8 sercom)
//{
//	/* 1) Enable peripheral and set clocks */
//
//	/* Enable SERCOM3 clock (APBCMASK) */
//	reg_set(PM_ADDR + 0x20, (1 << 5));
//	/* Set GCLK for SERCOM3 (generic clock generator 1) */
//	reg16_wr(GCLK_ADDR + 0x02, (1 << 14) | (1 << 8) | 23);
//
//	/* 2) Initialize UART block   */
//
//	/* Reset UART (set SWRST)     */
//	reg_wr((UART_ADDR + 0x00), 0x01);
//	/* Wait end of software reset */
//	while( reg_rd(UART_ADDR + 0x00) & 0x01)
//		;
//
//	/* Configure UART */
//	reg_wr(UART_ADDR + 0x00, 0x40100004);
//	reg_wr(UART_ADDR + 0x04, 0x00030000);
//	/* Configure Baudrate */
//	reg_wr(UART_ADDR + 0x0C, CONF_BAUD_RATE);
//
//	/* Set ENABLE into CTRLA */
//	reg_set( (UART_ADDR + 0x00), (1 << 1) );
//}

/**
 * @brief Send a single byte over UART
 *
 * @param c Character (or binary byte) to send
 */
void uart_putc(unsigned char c)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ((reg_rd(UART_ADDR + INTFLAG) & 0x01) == 0)
		;
	/* Write data */
	reg_wr((UART_ADDR + DATA), c);
}

/**
 * @brief Send a text-string over UART
 *
 * @param s Pointer to the null terminated text string
 */
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

/**
 * @brief Send the hexadecimal representation of a byte
 *
 * @param c Binary byte to show as hex
 */
void uart_puthex8(const u8 c)
{
	uart_putc(hex[(c >> 4) & 0xF]);
	uart_putc(hex[(c     ) & 0xF]);
}

/**
 * @brief Send the hexadecimal representation of a 16bits word
 *
 * @param c Binary word (16 bits) to show as hex
 */
void uart_puthex16(const u16 c)
{
	uart_puthex8(c >>  8);
	uart_puthex8(c >>  0);
}

/**
 * @brief Send the hexadecimal representation of a 32bits word
 *
 * @param c Binary word (32 bits) to show as hex
 */
void uart_puthex(const u32 c)
{
	uart_puthex8(c >> 24);
	uart_puthex8(c >> 16);
	uart_puthex8(c >>  8);
	uart_puthex8(c >>  0);
}

/**
 * @brief Send an hexadecimal dump of a buffer (or memory location)
 *
 * @param buffer Pointer to the memory location to dump
 * @param len    Number of bytes to dump
 */
void uart_dump(u8 *buffer, int len)
{
	u8 *p;
	int pos;
	int i;

	p = buffer;
	pos = 0;
	while(pos < len)
	{
		uart_puthex(pos);
		uart_putc(' ');

		for (i = 0; i < 16; i++)
		{
			uart_puthex8( *p );
			uart_putc(' ');
			p++;
			pos ++;
			if (pos == len)
				break;
		}
		uart_crlf();
	}
	uart_crlf();
}
/* EOF */
