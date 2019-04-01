#include "mcp2515.h"

/*
 * @brief Reset mcp2515 with pin
 */
int		can_hw_reset(void)
{
	u8 i;

	reg_wr((PORTA_ADDR + P_OUTTGL), 1 << 3);
	delay(2); /* must be at list 2us TODO check clock */
	reg_wr((PORTA_ADDR + P_OUTTGL), 1 << 3);
	delay(2);
	//while ((can_rd_reg(0x0e) != 0x80)) /* TODO find what to check*/
	//	if (++i == 0)				/* Try 255 times */
	//		return (1);
	return (0);
}

/*
 * @brief Reset mcp2515 over spi
 * @return Return 0 if reset succeed and 1 if it fails
 */
int		can_reset(void)
{
	u8 i;

	spi_cs(ON);
	spi_transfer(RESET);
	spi_cs(OFF);
	delay(2);
	//while ((can_rd_reg(0x0e) != 0x80)) /* TODO find what to check*/
	//	if (++i == 0)				/* Try 255 times */
	//		return (1);
	return (0);
}

/*
 * @brief Read arbitrary register
 * @param addr Register address to read
 * @param data Buffer where to write data
 * @param len length of the reading
 */
void	can_rd_reg(u8 addr, u8 *data, u8 len)
{
	u8 ret;

	spi_cs(ON);
	spi_transfer(READ);
	spi_transfer(addr);
	for (u32 i = 0; i < len; i++)
		data[i] = spi_transfer(0xFF);
	spi_cs(OFF);
}

/*
 * @brief Read RX buffer
 * @param buff BUFF0 or BUFF1
 * @param data Data to write
 * @param off  The offset where to start reading RXBnSIDH or RXBnD0
 */
void	can_rd_rx(u8 buff, u8 *data, u8 ptr, u8 len)
{
	spi_cs(ON);
	spi_transfer(RDRX | (buff << 2) | (ptr << 1));
	for (u8 i = 0; i < len; i++)
		data[i] = spi_transfer(0xFF);
	spi_cs(OFF);
}

/*
 * @brief Write data to register
 * @param addr Register address
 * @param data Data to write
 */
void	can_wr_reg(u8 addr, u8 *data, u8 len)
{
	spi_cs(ON);
	spi_transfer(WRITE);
	spi_transfer(addr);
	for (u8 i = 0; i < len; i++)
		spi_transfer(data[i]);
	spi_cs(OFF);
}

/*
 * @brief Write data to TX buffer
 * @param buff BUFF0 BUFF1 BUFF2
 * @param ptr TXBnSIDH TXBnD0
 */
void	can_ld_tx(u8 buff, u8 ptr, u8 data)
{
	spi_cs(ON);
	spi_transfer(LDTX | (buff << 1) | ptr);
	spi_transfer(data);
	spi_cs(OFF);
}

/*
 * @brief Send Request to send message
 * @param tx TXB0 TXB1 TXB2
 */
void	can_rts(u8 tx)
{
	spi_cs(ON);
	spi_transfer(RTS | tx);
	spi_cs(OFF);
}

/*
 * @brief Modify specific bit on compatible registers
 * @param addr Bit address to modify
 * @param mask Mask to apply
 * @param data Data to write
 */
void	can_bit_mod(u8 addr, u8 mask, u8 data)
{
	spi_cs(ON);
	spi_transfer(BITM);
	spi_transfer(addr);
	spi_transfer(mask);
	spi_transfer(data);
	spi_cs(OFF);
}

/*
 * @brief Read status octet
 *
 *  7 6 5 4 3 2 1 0
 *   \ \ \ \ \ \ \ \__RX0IF (CANINTF[0])
 *    \ \ \ \ \ \ \___RX1IF (CANINTF[1])
 *     \ \ \ \ \ \____TXREQ (TXB0CNTRL[3])
 *      \ \ \ \ \_____TX0IF (CANINTF[2])
 *       \ \ \ \______TXREQ (TXB1CNTRL[3])
 *        \ \ \_______TX1IF (CANINTF[3])
 *         \ \________TXREQ (TXB2CNTRL[3])
 *          \_________TX2IF (CANINTF[4])
 *  
 */
u8		can_rd_sta(void)
{
	u8	tmp;
	u8	ret;

	spi_cs(ON);
	spi_transfer(RDSTA);
	tmp = spi_transfer(0xFF);
	ret = spi_transfer(0xFF);
	spi_cs(OFF);
	if (ret == tmp)
		return (ret);
	else;
		// TODO What do I do here ???
}

/*
 * @brief Read several satus bits for transmit and receive functions
 */
u8		can_rx_sta(void)
{
	u8	tmp;
	u8	ret;

	spi_cs(ON);
	spi_transfer(RXSTA);
	tmp = spi_transfer(0xFF);
	ret = spi_transfer(0xFF);
	spi_cs(OFF);
	if (ret == tmp)
		return (ret);
	else;
		// TODO What do I do here ???
}

/*
 * @brief Debug output the mcp2515 memory
 */
void	dump_memory(u8 size)
{
	u8	can;

	uart_crlf();
	uart_puts("   ");
	uart_puts("\e[32m"); 
	for (u8 i = 0; i < size; i++)
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
		for (u8 j = 0; j < size; )
		{
			can_rd_reg(i, &can, 1);
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
			if ((size == 8 && i == 0x7f)
					|| (size == 16 && i == 0xff))
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
		if (size == 8)
			i -= 0x80;
		i++;
		uart_crlf();
	}
}

