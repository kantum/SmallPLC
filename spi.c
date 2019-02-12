#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"
#include "spi.h"

#define SPI_BAUD_RATE	((8000000 >> 1) / 9600) - 1

void	spi_put8(u8 hex)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 0) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTCLR, 1);
	reg16_wr((SPI_ADDR + DATA), ((hex & 0x000000ff) >>  0));
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 1) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 0);
}

void	spi_put16(u16 hex)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 0) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTCLR, 1);
	reg16_wr((SPI_ADDR + DATA), ((hex & 0x0000ff00) >>  8));
	reg16_wr((SPI_ADDR + DATA), ((hex & 0x000000ff) >>  0));
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 1) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 0);
}

void	spi_putx(u32 hex)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 0) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTCLR, 1);
	//* Write data */
	reg16_wr((SPI_ADDR + DATA), ((hex & 0xff000000) >> 24));
	reg16_wr((SPI_ADDR + DATA), ((hex & 0x00ff0000) >> 16));
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 1) == 0)
		;
	reg16_wr((SPI_ADDR + DATA), ((hex & 0x0000ff00) >>  8));
	reg16_wr((SPI_ADDR + DATA), ((hex & 0x000000ff) >>  0));
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 1) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 0);
}

void	spi_puts(u8 *str)
{
	u32	i;

	i = -1;
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 0x01) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTCLR, 1);
	//* Write data */
	while (str[++i])
	{
		reg16_wr((SPI_ADDR + DATA), str[i]);
		/* Wait Transmit Complete */
		while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 1) == 0)
			;
	}
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 0);
}

void	spi_putc(unsigned char c)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 0x01) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTCLR, 1);
	//* Write data */
	reg16_wr((SPI_ADDR + DATA), c);
	/* Wait Transmit Complete */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 1 << 1) == 0)
		;
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 0);
}

void	spi_init(u8 sercom)
{
	/* Configure Pins */
	reg8_wr((PORTB_ADDR + P_PINCFG + 0), 0);                  /* PB00 SCS */
	reg_wr(PORTB_ADDR + P_DIRSET, 1 << 0);
	reg8_wr((PORTB_ADDR + P_PINCFG + 1), 0x01);               /* PB01 SCK */
	reg8_wr((PORTB_ADDR + P_PINCFG + 2), 0x01);               /* PB02 MOSI */
	reg8_wr((PORTB_ADDR + P_PINCFG + 3), 0x01);               /* PB03 MISO */
	reg8_wr((PORTA_ADDR + P_PINCFG + 2), 0x01);               /* PA02 INT */
	reg8_wr((PORTA_ADDR + P_PINCFG + 3), 0x01);               /* PA03 RST */
	reg_wr((PORTB_ADDR + P_DIRSET), 1 << 31);                 /* PB31 PWR */
	/* Power up the PMOD Module */
	reg_wr(PORTB_ADDR + P_OUTCLR, 1 << 31);                   /* PB31 */
	/* Slave Select High */
	reg_wr(PORTB_ADDR + P_OUTSET, 1 << 0);
	/* Multiplexer for function D */
	reg8_wr((PORTB_ADDR + P_PMUX + 0), (0x03 << 4));          /* PB01 */
	reg8_wr((PORTB_ADDR + P_PMUX + 1), (0x03) | (0x03 << 4)); /* PB02 PB03 */
	reg8_wr((PORTA_ADDR + P_PMUX + 1), (0x03) | (0x03 << 4)); /* PA02 PA03 */
	/* Initialize sercom */
	sercom_init(sercom, 1);
	/* Reset SPI (set SWRST) */
	reg_wr((SPI_ADDR + CTRLA), 0x01);
	/* Wait end of software reset */
	while(reg_rd(SPI_ADDR + SYNCBUSY) & 0x01)
		;
	reg_wr(SPI_ADDR + CTRLA, (0 << 30)           /* DORD LSB first */
			|              (0x1 << 20)           /* DIPO PAD[1] PB03 */
			|              (0x3 << 16)           /* DOPO DO PAD[0] PB00 */
			                                     /* DOPO SCK PAD[3] PB01 */
			                                     /* DOPO SS PAD[1] PB03 */
			|              (0x3 << 2));          /* Mode SPI master */

	reg_wr(SPI_ADDR + CTRLB, (1 << 17));         /* RXEN Enable RX */	
	/* Wait for CTRLB */
	while(reg_rd((SPI_ADDR + SYNCBUSY)) & (1 << 2))
	/* Configure Baudrate */
	reg16_wr(SPI_ADDR + BAUD, SPI_BAUD_RATE);
	/* Set ENABLE into CTRLA */
	reg_set(SPI_ADDR, 0x02);
	while(reg_rd((SPI_ADDR + SYNCBUSY)) & (1 << 1))   /* Wait ENABLE busy */
		;
}
