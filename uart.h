/**
 * @file  uart.h
 * @brief Definitions and prototypes for UART functions
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
#ifndef UART_H
#define UART_H

#include "types.h"
#include "samd21.h"
#include "hardware.h"

void	uart_crlf(void);
void	uart_dump(u8 *d, int l);
void	uart_init(u8 sercom);
void	uart_putc(unsigned char c);
void	uart_puts(char *s);
void	uart_puthex(const u32 c);
void	uart_puthex8(const u8 c);
void	uart_puthex16(const u16 c);
void	uart_gets(u8 *str);
void	spi_putx(u32 hex);
void	uart_debug(u8 *str);

/*
 * TODO Create a new file for this functions
 */
void	sercom_init(u8 n, u8 clk);
void	clock_init(void);

#define UART_BAUD    	115200
#define UART_GCLK    	8000000
#define CONF_BAUD_RATE	(65536 - ((65536 * 16.0f * UART_BAUD) / UART_GCLK))

#endif
/* EOF */
