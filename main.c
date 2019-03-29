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

u8			test;
t_can_msg	receive[200];

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

void EIC_Handler(void)
{
	u32	flags;

	flags = reg_rd(EIC_ADDR + EIC_INTFLAG);
	if (flags & 0x1)
		uart_debug("0x1");
	else if (flags & 0x2)
		uart_debug("0x2");
	else if (flags & 0x3)
		uart_debug("0x3");
	else if (flags & 0x4)
		uart_debug("0x2");
	else if (flags & 0x5)
		uart_debug("0x5");
	else if (flags & 0x6)
		uart_debug("0x6");
	else if (flags & 0x7)
		uart_debug("0x7");

	u8 tmp;
	can_rd_reg(CANINTF, &tmp, 1);
	uart_puthex8(tmp);
	uart_crlf();
	can_wr_reg(CANINTF, "\x0", 1); 
	delay(10);
	uart_puthex8(tmp);
	uart_crlf();

	uart_puthex8(test);
	uart_crlf();
	uart_crlf();
	test++;

	reg_wr(EIC_ADDR + EIC_INTFLAG, 0x00);
}

void can_clear_register()
{
	for (u32 i = 0; i < 254; i++)
		can_wr_reg(i, "\x0", 1);
}

/**
 * @brief Don't you know main function ? Very handy one...
 */
void	main(void)
{
	u8			tmp[8];
	t_can_msg	colis;

	test = 0;
	leds_init();
	clock_init();
	uart_init(0);                                     /* UART init on sercom0 */
	spi_init(5);                                      /* SPI  init on sercom5 */
	can_init();
	interrupt_init(1);

	for (u8 i = 0; i < 8; i++)
		tmp[i] = 0x42;
	can_set_msg(&colis, 0x50F, 0b11, 0, 7, tmp);
	can_send(&colis);
	for (u32 i = 0; i < 200; i++)
		can_set_msg(&receive[i], 0x0, 0b0, 0, 0, tmp);
	while (1)
	{
		for (u32 i = 0; i < 10; i++)
			can_receive(&receive[i]);
		for (u32 i = 0; i < 10; i++)
		{
			for (u16 j = 0; j < receive[i].len; j++)
				uart_puthex8(receive[i].data[j]);
			uart_crlf();
		}
		uart_crlf();
		uart_puthex8(test);
		uart_crlf();
		reg_wr(PORTB_ADDR + P_OUTTGL, 1 << 10 | 1 << 11);
		dump_memory(USABLE);
		delay(1000);
	}
}
/* EOF */
