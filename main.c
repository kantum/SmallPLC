#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"
#include "spi.h"
#include "mcp2515.h"
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

/**
 * @brief CAN data structure
 */
typedef struct	s_can_msg
{
	u32	id;                            /* Can identifier              11 bits */
	u8	prio;                          /* Can priority                 2 bits */
	u8	rtr;                           /* Remote Transmission Request  1 bit  */
	u8	length;			               /* Can data length              8 bits */
	u8	data[8];
}				t_can_msg;

void	clear_can_registers(void)
{
	u8	tab[0xf];
	for (u8 i = 0; i < 0xf; i++)
		tab[i] = 0;

	for (u8 i = 0x00; i != 0x80; i += 0x10)
		can_wr_reg(i, tab, 0xe + 1);
}

void	can_init()
{
	if (can_reset() != 0)
	{
		uart_debug(C_YELLOW"Warning"C_END" failed, try to replug the module)");
		while (1);
	}
	uart_debug(C_GREEN"can_reset\r\n"C_END);
	//clear_can_registers();
	/* Set Baud Rate Prescaler bit TODO make a function out of this */
	can_wr_reg(CNF1, "\x03", 1);           /* BRP */
	                                       /* Tq = 2 x (BRP + 1)/Fosc */
	                                       /* Tq = 8/8000000 = 0.000001 = 1us */
	can_wr_reg(CNF2, "\x91", 1);           /* BTL mode to determined by PHSEG2*/
	                                       /* PHSEG  = 1 (PHSEG + 1) x Tq */
	                                       /* PRSEG  = 1 (PRSEG + 1) x Tq */
	can_wr_reg(CNF3, "\x03", 1);           /* PHSEG2 = 3 (PHSEG2 + 1) x Tq */
	                                       /* Minimum valid setting 2 TqS. */
	can_wr_reg(RXB0CTRL, "\x0", 1);   /* Receive any message, no filter/mask */
	can_wr_reg(RXB1CTRL, "\x0", 1);   /* Do not Receive any message, no filter/mask */
	can_wr_reg(CANINTF, "\x0", 1);                    /* Clear Interrupt Flags*/
	can_bit_mod(CANINTE, 0b11, 0b11);    /* RX0 and RX1 Interrupt Enable bits */
	can_wr_reg(TXB0CTRL | CANCTRL, "\x0", 1);    /* CAN Normal Operation mode */
}

void	can_tx_id(u32 id)
{
	u8	tmp;

	id = id & 0x7ff;                                     /* keep only 11 bits */
	tmp = (u8)(id >> 3);
	can_wr_reg(TXB0SIDH, &tmp, 1);                       /* identifier high   */
	tmp = (u8)((id & 0x7) << 5);
	can_wr_reg(TXB0SIDL, &tmp, 1);                       /* identifier low    */
}

void	can_rtr_dlc(t_can_msg msg)
{
	u8 tmp;

	tmp  = msg.rtr & 0x1;                /* RTR (Remote Transmission Request) */
	tmp |= msg.length & 0xf;                        /* DLC (Data Length Code) */
	can_wr_reg(TXB0DLC, &tmp, 1);        
}

void	can_send(t_can_msg msg)
{
	can_tx_id(msg.id);                                              /* ID can */
	can_bit_mod(TXB0CTRL, 0x3, (msg.prio & 0x3));            /* priority bits */
	can_bit_mod(TXB0CTRL | CANCTRL, 1 << 3, 1 << 3);        /* One shoot mode */
	can_rtr_dlc(msg);                                          /* RTR and DLC */
	can_wr_reg(TXB0D0, msg.data, msg.length);                    /* Send data */
	can_rts(TXB0);                                         /* Request to send */
}

void	can_rx_id(t_can_msg *msg)
{
	u16	tmp;

	tmp = can_rd_reg(RXB0SIDH) << 3;
	tmp |= (can_rd_reg(RXB0SIDL) & 0xe0) >> 5;
	msg->id = tmp;
}

u8		*can_receive(u8 *buffer)
{
	u16			i;
	u16			tmp;
	u16			tmp2;
	t_can_msg	receive;

	i = 0;
	//dump_memory(USABLE);
	can_get_error();
	while ((can_rd_reg(CANINTF) & 0x3 ) == 0)
	{
		uart_puts(".");
		if (++i > 1000)
		{
			uart_crlf();
			return(NULL);
		}
	}

	i = 0;
	uart_puts("\r\n  RX0 EID: ");
	can_rx_id(&receive);
	uart_puthex16(receive.id);
	uart_crlf();
	uart_puts("\r\n3 RX0 RXB0EID8: ");
	uart_puthex8(can_rd_reg(RXB0EID8));
	uart_puts("\r\n4 RX0 RXB0EID0: ");
	uart_puthex8(can_rd_reg(RXB0EID0));
	uart_puts("\r\n5 RX0  RXB0DLC: ");
	uart_puthex8(can_rd_reg(RXB0DLC));

	uart_crlf();
	uart_crlf();
	uart_puts("RX0 buffer\r\n");
	while (buffer[i] = can_rd_reg(RXB0D0 + i))
	{
		uart_puthex8(i);
		uart_puts("    ");
		uart_puthex8(buffer[i]);
		uart_crlf();
		i++;
	}

	return (buffer);
}

/**
 * @brief Don't you know main function ? Very handy one...
 */
int		main(void)
{
	u8			tmp[8];
	t_can_msg	colis;

	leds_init();
	clock_init();
	uart_init(0);                                     /* UART init on sercom0 */
	spi_init(5);                                      /* SPI  init on sercom5 */
	can_init();

	colis.id		= 0x50F;
	colis.prio		= 0b11;
	colis.rtr		= 0;
	colis.length	= 7;
	for (u8 i = 0; i < colis.length; i++)
		colis.data[i] 	= 0x42;

	uart_puts("send\r\n");
	dump_memory(USABLE);
	can_send(colis);
	can_receive(tmp);
	uart_puts("done\r\n");

	while (1)
	{
		uart_puts(".");
		reg_wr(PORTB_ADDR + P_OUTTGL, 1 << 10 | 1 << 11);
		delay(100);
	}
}
/* EOF */
