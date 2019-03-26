#include "can.h"

/**
 * @brief MCP2515 Configuration register filling with baud rate values
 */
void	can_baud_rate()
{
	u8 cnf1;
	u8 cnf2;
	u8 cnf3;

	cnf1 = SJW << 6 | BRP;                             /* SJW BRP             */
	cnf2 = BTL << 7 | SAM << 6 | PHSEG1 << 3 | PRSEG;  /* BTL SAM PHSEG PRSEG */
	cnf3 = SOF << 7 | WAKFIL << 6 | PHSEG2;            /* SOF WAKFIL PHSEG2   */

	can_wr_reg(CNF1, &cnf1, 1);                        /* CNF1 Register Write */
	can_wr_reg(CNF2, &cnf2, 1);                        /* CNF2 Register Write */
	can_wr_reg(CNF3, &cnf3, 1);                        /* CNF3 Register Write */
}

/**
 * @brief MCP2515 initialisation sequence
 */
void	can_init()
{
	if (can_reset() != 0)
	{
		uart_debug(C_YELLOW"Warning"C_END" failed, try to replug the module)");
		while (1);
	}
	uart_debug("\r\n"C_GREEN"can_reset\r\n"C_END);
	can_baud_rate();                             /* Baud Rate Configuration   */
	can_wr_reg(RXB0CTRL, "\x1", 1);              /* Receive Any Message       */
	can_wr_reg(CANINTF, "\x0", 1);               /* Clear Interrupt Flags     */
	can_bit_mod(CANINTE, 0x1, 0x1);              /* RX0 Interrupt Enable Bits */
	can_wr_reg(TXB0CTRL | CANCTRL, "\x0", 1);    /* CAN Normal Operation Mode */
}

/**
 * @brief Build the TXB0SIDH and TXB)SIDL registers from the ID value
 * @param id The CAN identifier
 *
 */
void	can_tx_id(u32 id)
{
	u8	tmp;

	id = id & 0x7ff;                                     /* Keep Only 11 Bits */
	tmp = (u8)(id >> 3);
	can_wr_reg(TXB0SIDH, &tmp, 1);                       /* Identifier High   */
	tmp = (u8)((id & 0x7) << 5);
	can_wr_reg(TXB0SIDL, &tmp, 1);                       /* Identifier Low    */
}

/**
 * @brief Merge RTR and DLC values to fit the register
 * @param msg A pointer to the CAN structure to set
 */
void	can_rtr_dlc(t_can_msg *msg)
{
	u8 tmp;

	tmp  = msg->rtr & 0x1;               /* RTR (Remote Transmission Request) */
	tmp |= msg->len & 0xf;               /* DLC (Data Length Code)            */
	can_wr_reg(TXB0DLC, &tmp, 1);        
}

/**
 * @brief Send a previously filled CAN structure
 * @param msg A pointer to the CAN structure to send
 */
void	can_send(t_can_msg *msg)
{
	can_tx_id(msg->id);                                    /* ID Can          */
	can_bit_mod(TXB0CTRL, 0x3, (msg->prio & 0x3));         /* Priority Bits   */
	can_bit_mod(TXB0CTRL | CANCTRL, 1 << 3, 1 << 3);       /* One Shoot Mode  */
	can_rtr_dlc(msg);                                      /* RTR And DLC     */
	can_wr_reg(TXB0D0, msg->data, msg->len);               /* Send Data       */
	can_rts(TXB0);                                         /* Request To Send */
}

/**
 * @brief Parse RXB0SIDH and RXB0SIDL to extract the received CAN identifier
 * @param msg A pointer to the CAN structure received
 */
void	can_rx_id(t_can_msg *msg, u8 *buf)
{
	msg->id = buf[RXB0SIDH & 0xF] << 3 | (buf[RXB0SIDL & 0xF] & 0xE0) >> 5;

	if ((buf[RXB0SIDL & 0xF] & (1 << 3)) != 0)           /* Check If Extended */
	{
		msg->ide = 1;
		if (buf[RXB0SIDL & 0xF] & (1 << 4))                  /* Check the SRR */
			msg->srr = 1; 
		msg->id <<= 16;
		msg->id |= (buf[RXB0EID8 & 0xF]) << 8 | buf[RXB0EID0 & 0xF];
	}
}

/**
 * @brief Parse received DLC
 * @param msg A pointer to the CAN structure received
 */
void	can_rx_len(t_can_msg *msg, u8 *buf)
{
	msg->len = buf[RXB0DLC & 0xF] & 0xF;
}

/**
 * @brief Fill Can Structure with parameters
 * @param msg A pointer to the structure to fille
 * @param id The can ID
 * @param prio priority bits
 * @param rtr Remote Transmission Request bit
 * @param len Data Length Code
 * @param data Data Array
 **/
void	can_set_msg(t_can_msg *msg, u16 id, u8 prio, u8 rtr, u8 len, u8 *data)
{
	msg->id     = id   & 0x7FF;
	msg->prio   = prio & 0x3;
	msg->rtr    = rtr  & 0x1;
	msg->len	= len;
	for (u8 i = 0; i < msg->len; i++)
		msg->data[i] = data[i];
}

/**
 * @brief Wait for a CAN message to be received
 * @param msg A pointer to the CAN structure
 */
void	can_receive(t_can_msg *msg)
{
	u32	i;
	u8	tmp;
	u8	buffer[0xf];

	i = -1;
	tmp = 0;
	do
	{
		can_rd_reg(CANINTF, &tmp, 1);
		if (++i > 1000)
		{
			uart_debug(C_YELLOW"[Warning]"C_END "can_receive time out");
			i = 0;
			return;
		}
	}
	while ((tmp & 0x1) == 0);
	can_rd_reg(RXB0CTRL, buffer, 0xf);
	can_rx_id(msg, buffer);
	can_rx_len(msg, buffer);
	i = -1;
	while (++i < 8)
	{
		msg->data[i] = buffer[(RXB0D0 & 0xF) + i];
	}
	can_bit_mod(CANINTF, 0x1, 0x0);               /* RX0 Interrupt Flag Clean */
}
/* EOF */
