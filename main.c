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
	uart_debug("Can message:");
	uart_puts("ID      : ");
	uart_puthex16(msg->id);
	uart_crlf();
	uart_puts("Priority: ");
	uart_puthex8(msg->prio);
	uart_crlf();
	uart_puts("rtr     : ");
	uart_puthex8(msg->rtr);
	uart_crlf();
	uart_puts("length  : ");
	uart_puthex8(msg->len);
	uart_crlf();
	uart_puts("data    : ");
	for (u16 i = 0; i < msg->len; i++)
		uart_puthex8(msg->data[i]);
	uart_crlf();
}

void can_set_msg(t_can_msg *msg, u16 id, u8 prio, u8 rtr, u8 len, u8 *data)
{
	msg->id     = id   & 0x7FF;
	msg->prio   = prio & 0x3;
	msg->rtr    = rtr  & 0x1;
	msg->len	= len;
	for (u8 i = 0; i < msg->len; i++)
		msg->data[i] = data[i];
}

/**
 * @brief Don't you know main function ? Very handy one...
 */
int		main(void)
{
	u8			tmp[8];
	t_can_msg	colis;
	t_can_msg	receive;

	leds_init();
	clock_init();
	uart_init(0);                                     /* UART init on sercom0 */
	spi_init(5);                                      /* SPI  init on sercom5 */
	can_init();


	for (u8 i = 0; i < 8; i++)
		tmp[i] = 0x42;
	can_set_msg(&colis, 0x50F, 0b11, 0, 7, tmp);

	can_set_msg(&receive, 0x0, 0b0, 0, 0, tmp);

	can_send(&colis);
	show_can_msg(&colis);
	uart_puts("send\r\n");
	can_receive(&receive);
	show_can_msg(&receive);
	uart_puts("done\r\n");

	while (1)
	{
		uart_puts(".");
		reg_wr(PORTB_ADDR + P_OUTTGL, 1 << 10 | 1 << 11);
		delay(100);
	}
}
/* EOF */
