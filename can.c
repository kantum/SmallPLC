#include "can.h"

extern u8 caninte;
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
	can_wr_reg(RXB1CTRL, "\x1", 1);              /* Receive Any Message       */
	can_wr_reg(CANINTF, "\x0", 1);               /* Clear Interrupt Flags     */
	can_wr_reg(TXB0CTRL | CANCTRL, "\x0", 1);    /* CAN Normal Operation Mode */
	caninte = 0x1;
	can_wr_reg(CANINTE, &caninte, 1);            /* RX0 Interrupt Enable Bits */
}

/**
 * @brief Build the TXB0SIDH and TXB)SIDL registers from the ID value
 * @param id The CAN identifier
 *
 */
void	can_tx_id(t_can_msg *msg)
{
	u8	tmp;

	if (msg->id > 0x7FF)
	{
		msg->id &= 0x1FFFFFFF;                           /* Keep Only 29 Bits */
		tmp = (u8)(msg->id & 0xFF);
		can_wr_reg(TXB0EID0, &tmp, 1);
		tmp = (u8)((msg->id & 0xFF00) >> 8);
		can_wr_reg(TXB0EID8, &tmp, 1);
		tmp = (u8)((msg->id & 0x30000) >> 16);
		tmp |= 0x8;                                      /* Set EXIDE bit     */
		tmp |= (u8)((msg->id & 0x1C0000) >> 13);
		can_wr_reg(TXB0SIDL, &tmp, 1);                   /* Identifier Low    */
		tmp = (u8)((msg->id & 0x1FE00000) >> 21);
		can_wr_reg(TXB0SIDH, &tmp, 1);                   /* Identifier High   */
	}
	else
	{
		msg->id &= 0x7FF;                                /* Keep Only 11 Bits */
		tmp = (u8)(msg->id >> 3);
		can_wr_reg(TXB0SIDH, &tmp, 1);                   /* Identifier High   */
		tmp = (u8)((msg->id & 0x7) << 5);
		can_wr_reg(TXB0SIDL, &tmp, 1);                   /* Identifier Low    */
	}
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
	can_tx_id(msg);                                    /* ID Can          */
	can_bit_mod(TXB0CTRL, 0x3, (msg->prio & 0x3));         /* Priority Bits   */
	can_bit_mod(TXB0CTRL | CANCTRL, 1 << 3, 1 << 3);       /* One Shoot Mode  */
	can_rtr_dlc(msg);                                      /* RTR And DLC     */
	can_wr_reg(TXB0D0, msg->data, msg->len);               /* Send Data       */
	can_rts(TXB0);                                         /* Request To Send */
	can_bit_mod(CANINTF, 0x4, 0x0);                        /* Clean flag      */
}


/**
 * @brief Parse mcp2515 registers to extract the received CAN identifier
 * @param msg A pointer to the CAN structure received
 * @param buf Which buffer to read, BUFF0 or BUFF1
 */
void	can_rx_id(t_can_msg *msg, u8 *buf)
{
	msg->id = ((u32)buf[0] << 3)                         /* RXBnSIDH[7-0]     */
			| ((u32)(buf[1]) >> 5);                      /* RXBnSIDL[7-4]     */

	if ((buf[1] & (1 << 3)) != 0)                        /* Check If Extended */
	{
		msg->ide = 1;
		if (buf[1] & (1 << 4))                           /* Check the SRR     */
			msg->srr = 1;
		msg->id <<= 18;
		msg->id |= ((buf[1] & 0x3) << 16)                /* RXBnSIDL[1-0]     */
				| (buf[2] << 8)                          /* RXBnEID8[7-0]     */
				| buf[3];                                /* RXBnEID0[7-0]     */
	}
}

/**
 * @brief Parse received DLC
 * @param msg A pointer to the CAN structure received
 */
void	can_rx_len(t_can_msg *msg, u8 *buf)
{
	msg->len = buf[4] & 0xF;
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
void	can_set_msg(t_can_msg *msg, u32 id, u8 prio, u8 rtr, u8 len, u8 *data)
{
	msg->id     = id;
	msg->prio   = prio & 0x3;
	msg->rtr    = rtr  & 0x1;
	msg->len	= len;
	for (u8 i = 0; i < msg->len; i++)
		msg->data[i] = data[i];
}

/**
 * @brief Wait for a CAN message to be received
 * @param msg  A pointer to the CAN structure
 * @param buff Buffer register to read BUFF0 BUFF1
 */
void	can_receive(t_can_msg *msg, u8 buff)
{
	u32	i;
	u8	tmp;
	u8	buffer[0xf];

	i = -1;
	tmp = 0;
	can_rd_rx(buff, buffer, RXBnSIDH, 0xD);
	can_rx_id(msg, buffer);
	can_rx_len(msg, buffer);
	i = -1;
	while (++i < 8)
		msg->data[i] = buffer[5 + i];
}
/* EOF */
