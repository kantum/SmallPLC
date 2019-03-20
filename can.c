#include "can.h"

void	can_baud_rate()
{
	/* Set Baud Rate Prescaler bit TODO make a function out of this */
	can_wr_reg(CNF1, "\x03", 1);           /* BRP */
	                                       /* Tq = 2 x (BRP + 1)/Fosc */
	                                       /* Tq = 8/8000000 = 0.000001 = 1us */
	can_wr_reg(CNF2, "\x91", 1);           /* BTL mode to determined by PHSEG2*/
	                                       /* PHSEG  = 1 (PHSEG + 1) x Tq */
	                                       /* PRSEG  = 1 (PRSEG + 1) x Tq */
	can_wr_reg(CNF3, "\x03", 1);           /* PHSEG2 = 3 (PHSEG2 + 1) x Tq */
	                                       /* Minimum valid setting 2 TqS. */
}

void	can_init()
{
	if (can_reset() != 0)
	{
		uart_debug(C_YELLOW"Warning"C_END" failed, try to replug the module)");
		while (1);
	}
	uart_debug(C_GREEN"can_reset\r\n"C_END);
	can_baud_rate();
	can_wr_reg(RXB0CTRL, "\x1", 1);    /* Receive any message, no filter/mask */
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

void	can_rtr_dlc(t_can_msg *msg)
{
	u8 tmp;

	tmp  = msg->rtr & 0x1;               /* RTR (Remote Transmission Request) */
	tmp |= msg->len & 0xf;                       /* DLC (Data Length Code) */
	can_wr_reg(TXB0DLC, &tmp, 1);        
}

void	can_send(t_can_msg *msg)
{
	can_tx_id(msg->id);                                             /* ID can */
	can_bit_mod(TXB0CTRL, 0x3, (msg->prio & 0x3));           /* priority bits */
	can_bit_mod(TXB0CTRL | CANCTRL, 1 << 3, 1 << 3);        /* One shoot mode */
	can_rtr_dlc(msg);                                          /* RTR and DLC */
	can_wr_reg(TXB0D0, msg->data, msg->len);                  /* Send data */
	can_rts(TXB0);                                         /* Request to send */
}

void	can_rx_id(t_can_msg *msg)
{
	u16	tmp;

	msg->id = (u16)can_rd_reg(RXB0SIDH) << 3
		| (can_rd_reg(RXB0SIDL) & 0xE0) >> 5;
	if ((tmp = can_rd_reg(RXB0SIDL) & (1 << 3)) != 0)    /* Check if extended */
	{
		msg->ide = 1;
		if (tmp & (1 << 4))       /* Check the SRR */
			msg->srr = 1; 
		msg->id <<=16;
		msg->id |= (can_rd_reg(RXB0EID8) << 8) | (can_rd_reg(RXB0EID0));
	}
}

void	can_rx_len(t_can_msg *msg)
{
	msg->len = can_rd_reg(RXB0DLC) & 0xF;
}

void	can_receive(t_can_msg *msg)
{
	u16	i;
	u8	*buffer;

	i = -1;
	while ((can_rd_reg(CANINTF) & 0x3 ) == 0)
	{
		if (++i > 1000)
		{
			uart_debug(C_YELLOW"[Warning]"C_END "can_receive time out");
			return;
		}
	}
	can_rx_id(msg);
	can_rx_len(msg);
	i = -1;
	while (++i < 8)
	{
		msg->data[i] = can_rd_reg(RXB0D0 + i);
	}
}
