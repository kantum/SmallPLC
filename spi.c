#include "types.h"
#include "hardware.h"
#include "samd21.h"
//#include "uart.h"
#include "spi.h"

void	spi_flush_rx(void)
{
	reg16_rd((SPI_ADDR + DATA));
}

void	spi_wait_rx(void)
{
	/* Read INTFLAG and wait RXC (Receive Complete) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 2) == 0)
		;
}

void	spi_wait_tx(void)
{
	/* Read INTFLAG and wait TXC (Transmit Complete) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 1) == 0)
		;
}

void	spi_cs(u8 status)
{
	if (status == ON)
		reg_wr(PORTB_ADDR + P_OUTCLR, 1 << 0);
	else if (status == OFF)
	{
		//spi_wait_rx();
		//spi_wait_tx();
		reg_wr(PORTB_ADDR + P_OUTSET, 1 << 0);
	}
	else;
}

u8		spi_read8(void)
{
	reg16_wr((SPI_ADDR + DATA), 0x0000);
	spi_wait_rx();
	return(reg16_rd((SPI_ADDR + DATA)));
}

void	spi_put8(u8 hex)
{
	reg16_wr((SPI_ADDR + DATA), hex );
	spi_wait_tx();
	spi_wait_rx();
	spi_flush_rx();
}

void	spi_put16(u16 hex)
{
	spi_put8((hex & 0x0000ff00) >>  8);
	spi_put8((hex & 0x000000ff) >>  0);
}

void	spi_putx(u32 hex)
{
	spi_put8((hex & 0xff000000) >> 24);
	spi_put8((hex & 0x00ff0000) >> 16);
	spi_put8((hex & 0x0000ff00) >>  8);
	spi_put8((hex & 0x000000ff) >>  0);
}

/*
 * @brief Initialize spi module
 * @param sercom Select which sercom to use
 */
void	spi_init(u8 sercom)
{
	/* Configure Pins */
	reg8_wr((PORTB_ADDR + P_PINCFG + 0), 0);         /* PB00 SCS */
	reg_wr(PORTB_ADDR + P_DIRSET, 1 << 0);
	reg8_wr((PORTB_ADDR + P_PINCFG + 1), 0x01);      /* PB01 SCK */
	reg8_wr((PORTB_ADDR + P_PINCFG + 2), 0x01);      /* PB02 MOSI */
	reg8_wr((PORTB_ADDR + P_PINCFG + 3), 0x01);      /* PB03 MISO */
	reg_wr((PORTB_ADDR + P_DIRSET), 1 << 31);        /* PB31 PWR */
	reg_wr(PORTB_ADDR + P_OUTCLR, 1 << 31);          /* Power up PMOD Module */
	/* Multiplexer for function D */
	reg8_wr((PORTB_ADDR + P_PMUX + 0), (0x03 << 4)); /* PB01 */
	reg8_wr((PORTB_ADDR + P_PMUX + 1), (0x03)        /* PB02 */
			|  (0x03 << 4)); /* PB03 */
	sercom_init(sercom, 1);                          /* Initialize sercom */
	reg_wr((SPI_ADDR + CTRLA), 0x01);                /* Reset SPI */
	while(reg_rd(SPI_ADDR + SYNCBUSY) & 0x01)        /* Wait end of Reset */
		;
	reg_wr(SPI_ADDR + CTRLA, (0x1 << 20) | (0x3 << 16) | (0x3 << 2));          
	/*                            /             /            /     */
	/* DIPO PAD[1] PB03 ---------/             /            /      */
	/* DOPO DO PAD[0] PB02 -------------------/            /       */
	/* DOPO SCK PAD[3] PB01 -----------------/            /        */
	/* DOPO SS PAD[1] PB03 -----------------/            /         */
	/* Mode SPI master ---------------------------------/          */

	reg_wr(SPI_ADDR + CTRLB, (1 << 17));             /* RXEN Enable RX */	
	while(reg_rd((SPI_ADDR + SYNCBUSY)) & (1 << 2))  /* Wait for CTRLB */
		;
	reg16_wr(SPI_ADDR + BAUD, SPI_BAUD_RATE);        /* Configure Baudrate */
	reg_set(SPI_ADDR + CTRLA, 0x02);                 /* SERCOM SPI ENABLE */
	while(reg_rd((SPI_ADDR + SYNCBUSY)) & (1 << 1))  /* Wait ENABLE busy */
		;
}
