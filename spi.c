#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"
#include "spi.h"

void	spi_putc(unsigned char c)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ((reg_rd(SPI_ADDR + INTFLAG) & 0x01) == 0)
		;
	/* Write data */
	reg16_wr((SPI_ADDR + DATA), c);
}

void	spi_init(u8 sercom)
{
	/* Configure Pins */
	reg8_wr((PORTB_ADDR + P_PINCFG + 0), 0x01);               /* PB00 SCS */
	reg8_wr((PORTB_ADDR + P_PINCFG + 1), 0x01);               /* PB01 SCK */
	reg8_wr((PORTB_ADDR + P_PINCFG + 2), 0x01);               /* PB02 MOSI */
	reg8_wr((PORTB_ADDR + P_PINCFG + 3), 0x01);               /* PB03 MISO */

	reg8_wr((PORTA_ADDR + P_PINCFG + 2), 0x01);               /* PA02 INT */
	reg8_wr((PORTA_ADDR + P_PINCFG + 3), 0x01);               /* PA03 RST */
	reg_wr((PORTB_ADDR + P_DIRSET), 1 << 31);                 /* PB31 PWR */

	/* Multiplexer for function D */
	reg8_wr((PORTB_ADDR + P_PMUX + 0), (0x03) | (0x03 << 4)); /* PB00 PB01 */
	reg8_wr((PORTB_ADDR + P_PMUX + 1), (0x03) | (0x03 << 4)); /* PB02 PB03 */
	reg8_wr((PORTA_ADDR + P_PMUX + 1), (0x03) | (0x03 << 4)); /* PA02 PA03 */

	/* Initialize sercom */
	sercom_init(sercom, 1);

	/* Reset SPI (set SWRST) */
	reg_wr((SPI_ADDR + CTRLA), 0x01);

	/* Wait end of software reset */
	while(reg_rd(SPI_ADDR + SYNCBUSY) & 0x01)
		;
	reg_wr(SPI_ADDR + CTRLA, 0x40000000           /* DORD LSB first */
			| 0x100000                            /* DIPO PAD[1] PB03 */
			| 0x30000                             /* DOPO DO PAD[0] PB00 */
			                                      /* DOPO SCK PAD[3] PB01 */
			                                      /* DOPO SS PAD[1] PB03 */
			| 0x08);                              /* Mode SPI master */

	reg_wr(SPI_ADDR + CTRLB, 1 << 13               /* MSSEN Slave Select */
			| 1 << 17);                           /* RXEN Enable RX */
	while(reg_rd((SPI_ADDR + SYNCBUSY)) & (1 << 2))   /* Wait CTRLB busy */
			;

	/* TODO fucking baud rate */
	/* Configure Baudrate */
	//reg8_wr(SPI_ADDR + BAUD, 0x0F);

	uart_puts("baud register: ");
	uart_puthex(reg_rd(SPI_ADDR + BAUD));
	uart_crlf();

	/* Power up the PMOD Module */
	reg_set(PORTB_ADDR + P_OUTCLR, 1 << 31);                  /* PB31 */
	/* Set ENABLE into CTRLA */
	reg_set(SPI_ADDR, 0x02);
	while(reg_rd((SPI_ADDR + SYNCBUSY)) & (1 << 1))   /* Wait ENABLE busy */
		;
}
