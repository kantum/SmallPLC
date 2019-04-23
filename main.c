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

t_can_msg	receive[200];
u32			counter;
u32			flags;
u8			can_flags;
u8			eflg;
u8			caninte;

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
	//	uart_puts("data    : ");
	for (u16 i = 0; i < msg->len; i++)
		uart_puthex8(msg->data[i]);
	uart_crlf();
}

void	uart_clear(void)
{
	uart_puts("\x1b[2J");
}

#define RX0IF	1 << 0
#define RX1IF	1 << 1
#define TX0IF	1 << 2
#define TX1IF	1 << 3
#define TX2IF	1 << 4
#define ERRIF	1 << 5
#define WAKIF	1 << 6
#define MERRF	1 << 7

#define EWARN	1 << 0
#define RXWAR	1 << 1
#define TXWAR	1 << 2
#define RXEP	1 << 3
#define TXEP	1 << 4
#define TXBO	1 << 5
#define RX0OVR	1 << 6
#define RX1OVR	1 << 7

/*
 * @brief Interrupt code
 */
void EIC_Handler(void)
{
	flags = reg_rd(EIC_ADDR + EIC_INTFLAG);
	if (flags & (1 << 2))
	{
		can_rd_reg(CANINTF, &can_flags, 1);
		if (can_flags & RX0IF)
			can_receive(&receive[counter], BUFF0);
		if (can_flags & RX1IF)
			can_receive(&receive[counter], BUFF1);
		if (can_flags & ERRIF)
		{
			can_rd_reg(EFLG, &eflg, 1);
			if (eflg & EWARN)
				uart_debug("EWARN");
            if (eflg & RXWAR)
				uart_debug("RXWAR");
            if (eflg & TXWAR)
				uart_debug("TXWAR");
            if (eflg & RXEP)
				uart_debug("RXEP");
            if (eflg & TXEP)
				uart_debug("TXEP");
            if (eflg & TXBO)
				uart_debug("TXBO");
            if (eflg & RX0OVR)
				uart_debug("RX0OVR");
            if (eflg & RX1OVR)
				uart_debug("RX1OVR");
			can_wr_reg(CANINTF, "\x0", 1);
		}
		counter++;
	}
	reg_wr(EIC_ADDR + EIC_INTFLAG, flags);
	if (counter == 200)
	{
		//reg_wr(EIC_ADDR + EIC_INTENCLR, (1 << 2)); /* Disable EXTINT2 for EIC */
		can_wr_reg(CANINTE, "\x0", 1);

		for (u32 i = 0; i < 200; i++)
			show_can_msg(&receive[i]);
		counter = 0; 
		//reg_wr(EIC_ADDR + EIC_INTENSET, (1 << 2));  /* Enable EXTINT2 for EIC */
		can_wr_reg(CANINTE, &caninte, 1);
	}
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

	leds_init();
	clock_init();
	uart_init(0);                                     /* UART init on sercom0 */
	spi_init(5);                                      /* SPI  init on sercom5 */
	interrupt_init(1);
	can_init();

	counter = 0;
	for (u8 i = 0; i < 8; i++)
		tmp[i] = 0x42;
	can_set_msg(&colis, 0x1AAAAAAA, 0b11, 0, 7, tmp);
	//can_set_msg(&colis, 0x1FF, 0b11, 0, 7, tmp);
	for (u32 i = 0; i < 200; i++)
		can_set_msg(&receive[i], 0x0, 0b0, 0, 0, tmp);
	can_send(&colis);
	while (1)
	{
		reg_wr(PORTB_ADDR + P_OUTTGL, 1 << 10 | 1 << 11);
		delay(10);
	}
}
/* EOF */
