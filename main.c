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

void	dump_memory(void)
{
	u8 can;

	for (u8 i = 0; i < 254; i++)
	{
		for (u8 j = 0; j < 32; j++)
		{
			for (u8 k = 0; k < 2; k++)
			{
				can = can_rd_reg(i);
				uart_puthex8(can);
			}
			uart_putc(' ');
		}
		uart_crlf();
	}
}

/**
 * @brief Don't you know main function ? Very handy one...
 */
int		main(void)
{
	unsigned char	str[100];

	leds_init();
	clock_init();
	uart_init(0);                  /* UART init on sercom0 */
	spi_init(5);                   /*  SPI init on sercom5 */
	can_reset();
	//can_wr_reg(0x00, 0xff);
	//can_rd_reg(0x00);
	//can_wr_reg(0x00, 0x0f);
	//can_rd_reg(0x00);
	//can_wr_tx(BUFF0, TXBnSIDH, 0x2a);
	//can_rts(TXB0);
	//uart_puthex8(can_rd_sta());
	//uart_puthex8(can_rd_sta());
	//uart_puthex8(can_rd_sta());
	while (1)
	{
	}
}
/* EOF */
