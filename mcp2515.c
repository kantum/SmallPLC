#include "types.h"
#include "spi.h"
#include "uart.h"
#include "mcp2515.h"

/*
 * @brief Reset mcp2515 over spi
 */
void	can_reset(void)
{
	spi_cs(ON);
	spi_put8(RESET);
	spi_cs(OFF);
	/* TODO Avoid 3 repeat */
	while (can_rd_sta() != 0)
		;
}

/*
 * @brief Read arbitrary register
 * @param addr Register address to read
 */
u8		can_rd_reg(u8 addr)
{
	u8 ret;

	spi_cs(ON);
	spi_put16((READ << 8) | addr);
	spi_wait_tx();
	ret = spi_read8();
	spi_cs(OFF);
	return (ret);
}

/*
 * @brief Read RX buffer
 * @param buff BUFF0 or BUFF1
 * @param ptr RXBnSIDH or RXBnD0
 */
u8		can_rd_rx(u8 buff, u8 ptr)
{
	u8	ret;

	spi_cs(ON);
	spi_put8(RDRX | (buff << 2) | (ptr << 1));
	spi_wait_tx();
	ret = spi_read8();
	spi_cs(OFF);
	return (ret);
}

/*
 * @brief Write data to register
 * @param addr Register address
 * @param data Data to write
 */
void	can_wr_reg(u8 addr, u8 data)
{
	spi_cs(ON);
	spi_put8(WRITE);
	spi_put8(addr);
	spi_wait_tx();
	spi_put8(data);
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
	spi_put8(LDTX | (buff << 1) | ptr);
	spi_put8(data);
	spi_cs(OFF);
}

/*
 * @brief Send Request to send message
 * @param tx TXB0 TXB1 TXB2
 */
void	can_rts(u8 tx)
{
	spi_cs(ON);
	spi_put8(RTS | tx);
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
	spi_putx(BITM << 24 | addr << 16 | mask << 8 | data);
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
	spi_put8(RDSTA);
	tmp = spi_read8();
	spi_wait_tx();
	ret = spi_read8();
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
	spi_put8(RXSTA);
	tmp = spi_read8();
	spi_wait_tx();
	ret = spi_read8();
	spi_cs(OFF);
	if (ret == tmp)
		return (ret);
	else;
		// TODO What do I do here ???
}
