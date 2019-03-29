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
#include "uart.h"

//TODO Clean this file

static const u8 hex[16] = "0123456789ABCDEF";

/**
 * @brief Send end-of-line string CR-LF over UART
 *
 */
void uart_crlf(void)
{
	uart_puts("\r\n");
}

/*
 * @brief Initialize GCLK at 8Mhz
 */
void	clock_init(void)
{
	/* Set Divisor GCLK1 : enabled, OSC8M, no divisor */
	reg_wr(GCLK_ADDR + GCLK_GENDIV, (1 << 8) | 0x01);

	/* Generic Clock Generator Enable */
	reg_wr(GCLK_ADDR + GCLK_GENCTRL, (1 << 16)             /* Enable          */
			| (0x06 << 8)					               /* Source -> OSC8M */
			| 0x01);                                       /* Select GCLK1    */
}

/**
 * @brief Initialize and configure UART port
 * @param 
 */
void	uart_init(u8 sercom)
{
	/* Configure Pins */
	reg8_wr((PORTA_ADDR + P_PINCFG + 8), 0x01);                       /* PA08 */
	reg8_wr((PORTA_ADDR + P_PINCFG + 9), 0x01);                       /* PA09 */

	/* Multiplexer for function C */
	reg8_wr((PORTA_ADDR + P_PMUX + 4), (0x02) | (0x02 << 4));
	reg8_wr((PORTA_ADDR + P_PMUX + 5), (0x02) | (0x02 << 4));

	/* Initialize sercom */
	sercom_init(sercom, 1);

	/* Reset UART (set SWRST) */
	reg_wr((UART_ADDR + CTRLA), 0x01);

	/* Wait end of software reset */
	while(reg_rd(UART_ADDR + SYNCBUSY) & 0x01)
		;
	reg_wr(UART_ADDR + CTRLA, 0x40000000           /* DORD LSB first          */
			| 0x100000                             /* RXPO PAD[1] TXPO PAD[0] */
			| 0x04);                               /* Internal Clock          */
	/* Enable TX and RX */
	reg_wr(UART_ADDR + CTRLB, 0x00030000);

	/* Configure Baudrate */
	reg16_wr(UART_ADDR + BAUD, CONF_BAUD_RATE);

	/* Power up the PMOD Module */
	reg_wr(PORTA_ADDR + P_DIRSET, 1 << 4);
	reg_wr(PORTA_ADDR + P_OUTCLR, 1 << 4);

	/* Set ENABLE into CTRLA */
	reg_set(UART_ADDR, 0x02);
}

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
	reg16_wr((UART_ADDR + DATA), c);                            /* Write data */
}

/**
 * @brief Send a text-string over UART
 *
 * @param s Pointer to the null terminated text string
 */
void uart_puts(char *s)
{
	while(*s)
	{
		uart_putc(*s);
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

/**
 * @brief Put the octet from DATA to *str
 * @param str The addresse where to put the data
 */
void	uart_gets(u8 *str)
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
		if (str[i] == '\n'
				|| str[i] == '\0'
				|| str[i] == '\r')
			break;
		i++;
	}
	str[i] = '\0';
}

void	uart_debug(u8 *str)
{
	uart_puts(str);
	uart_crlf();
}
/* EOF */
