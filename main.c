#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"
#include "spi.h"
#include "mcp2515.h"
#include "can.h"
#include "delay.h"
#include "color.h"
#include "error.h"

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

void	show_can_msg(t_can_msg *msg)
{
//	uart_debug("Can message:");
//	uart_puts("ID      : ");
//	uart_puthex16(msg->id);
//	uart_crlf();
//	uart_puts("Priority: ");
//	uart_puthex8(msg->prio);
//	uart_crlf();
//	uart_puts("rtr     : ");
//	uart_puthex8(msg->rtr);
//	uart_crlf();
//	uart_puts("length  : ");
//	uart_puthex8(msg->len);
//	uart_crlf();
	uart_puts("data    : ");
	for (u16 i = 0; i < msg->len; i++)
		uart_puthex8(msg->data[i]);
	uart_crlf();
}

void	uart_clear(void)
{
	uart_puts("\x1b[2J");
}

/**
 * @brief Don't you know main function ? Very handy one...
 */
void	main(void)
{
	u8			tmp[8];
	t_can_msg	colis;
	t_can_msg	receive[500];

	leds_init();
	clock_init();
	uart_init(0);                                     /* UART init on sercom0 */
	spi_init(5);                                      /* SPI  init on sercom5 */
	can_init();

	for (u8 i = 0; i < 8; i++)
		tmp[i] = 0x42;

	can_set_msg(&colis, 0x50F, 0b11, 0, 7, tmp);

	can_send(&colis);

	uart_clear();
	for (u32 i = 0; i < 500; i++)
		can_set_msg(&receive[i], 0x0, 0b0, 0, 0, tmp);
	while (1)
	{
		for (u32 i = 0; i < 500; i++)
			can_receive(&receive[i]);
		for (u32 i = 0; i < 500; i++)
		{
			for (u16 j = 0; j < receive[i].len; j++)
				uart_puthex8(receive[i].data[j]);
			uart_crlf();
		}
		uart_crlf();
		reg_wr(PORTB_ADDR + P_OUTTGL, 1 << 10 | 1 << 11);
	}
}
/* EOF */
