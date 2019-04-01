#ifndef SPI_H
#define SPI_H

#include "types.h"
#include "hardware.h"
#include "samd21.h"

#define SPI_BAUD_RATE	((8000000 >> 1) / 2000000) - 1      /* Baud rate 2Mhz */

#define	ON	(u8)1
#define	OFF	(u8)0

void	spi_init(u8 sercom);
void	spi_put8(u8 hex);
void	spi_put16(u16 hex);
void	spi_putx(u32 hex);
void	spi_cs(u8 status);
u8		spi_read8(void);
void	spi_wait_tx(void);
void	spi_wait_rx(void);
u8		spi_transfer(u8 data);
void	spi_flush_rx(void);

#endif

