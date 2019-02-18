#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"
#include "spi.h"
#include "mcp2515.h"

/**
 * @brief Initialize status leds
 * @section Led handling
 */
void	leds_init()
{
	/* Forget bootloader configuration for PB10 */
	reg8_wr((PORTB_ADDR + P_PINCFG + 10), 0);
	/* Configure PB10 and PB11 as output (DIR) */
	reg_wr(PORTB_ADDR + P_DIRSET, 1 << 10 | 1 << 11);
	/* Turn them off*/
	reg_wr(PORTB_ADDR + P_OUTCLR, 1 << 10 | 1 << 11);
}

/*
 * @brief Debug output the mcp2515 memory
 */
void	dump_memory(void)
{
	u8 can;

	uart_crlf();
	uart_puts("   ");
	uart_puts("\e[32m"); 
	for (u8 i = 0; i < 16; i++)
	{
		uart_putc(' ');
		uart_puthex8(i << 4);
	}
	uart_puts("\e[0m");
	uart_crlf();
	uart_crlf();
	for (u16 i = 0; i <= 254; )
	{
		uart_puts("\e[32m"); 
		uart_puthex8(i);
		uart_puts("\e[0m");
		uart_putc(' ');
		uart_putc(' ');
		for (u8 j = 0; j < 16; )
		{
			can = can_rd_reg(i);
			if (can != 0)
			{
				uart_puts("\e[33m"); 
				uart_puthex8(can);
				uart_puts("\e[0m");
			}
			else
			{
				uart_puts("\e[34m"); 
				uart_puthex8(can);
				uart_puts("\e[0m");
			}
			if (i == 0xff)
			{
				uart_crlf();
				uart_crlf();
				return;
			}
			i += 0x10;
			i &= 0xff;
			j++;
			uart_putc(' ');
		}
		i++;
		uart_crlf();
	}
}

typedef struct
{
	u32			id;
	struct
	{
		u8	rtr : 1;
		u8	length : 4;
	}			header;

	u8 data[8];
}can_msg;

/**
 * @brief Don't you know main function ? Very handy one...
 */
int		main(void)
{
	leds_init();
	clock_init();
	uart_init(0);                  /* UART init on sercom0 */
	spi_init(5);                   /*  SPI init on sercom5 */

	can_reset();
	//for (u16 i; i < 255; i++)
	//	can_wr_reg(i, 0x00);
	dump_memory();
	//can_wr_reg(TXB0CTRL, 1 << 3);     /* TXREQ */
	// TODO function to place identifier
	can_wr_reg(TXB0SIDH, 0xff);       /* Identifier High */
	dump_memory();
	can_wr_reg(TXB0SIDL, 0b111 << 5); /* Identifier Low */
	dump_memory();
	can_wr_reg(TXB0DLC, 0x1);         /* 1 in DLC (Data Length Code) */
	can_wr_reg(CANINTE, 1 << 2);      /* Transmit Buffer 0 Empty Interrupt Enable bit */
	can_rts(TXB0);
	while (can_rd_reg(CANINTF))
		;
	dump_memory();
	uart_crlf();
	uart_crlf();
	uart_puthex8(can_rd_reg(CANINTF));
	//can_reset();
	//

	//dump_memory();
	while (1)
	{
	}
}
/* EOF */
