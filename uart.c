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

#define UART_ADDR		((u32)0x42000800)
#define UART_BAUD    9600
#define UART_GCLK 8000000
#define CONF_BAUD_RATE  (65536 - ((65536 * 16.0f * UART_BAUD) / UART_GCLK))

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
void	init_sercom(u8 n, u8 clk)
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
void uart_init(u8 sercom)
{
	/* 1) Enable peripheral and set clocks */

	/* Enable SERCOM3 clock (APBCMASK) */
	reg_set(PM_ADDR + 0x20, (1 << 5));
	/* Set GCLK for SERCOM3 (generic clock generator 1) */
	reg16_wr(GCLK_ADDR + 0x02, (1 << 14) | (1 << 8) | 23);

	/* 2) Initialize UART block   */

	/* Reset UART (set SWRST)     */
	reg_wr((UART_ADDR + 0x00), 0x01);
	/* Wait end of software reset */
	while( reg_rd(UART_ADDR + 0x00) & 0x01)
		;

	/* Configure UART */
	reg_wr(UART_ADDR + 0x00, 0x40100004);
	reg_wr(UART_ADDR + 0x04, 0x00030000);
	/* Configure Baudrate */
	reg_wr(UART_ADDR + 0x0C, CONF_BAUD_RATE);

	/* Set ENABLE into CTRLA */
	reg_set( (UART_ADDR + 0x00), (1 << 1) );
}

/**
 * @brief Send a single byte over UART
 *
 * @param c Character (or binary byte) to send
 */
void uart_putc(unsigned char c)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ( (reg_rd(UART_ADDR + 0x18) & 0x01) == 0)
		;
	/* Write data */
	reg_wr((UART_ADDR + 0x28), c);
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
	uart_putc( hex[(c >>  4) & 0xF] );
	uart_putc( hex[(c      ) & 0xF] );
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
