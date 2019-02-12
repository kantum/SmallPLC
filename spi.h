#ifndef SPI_H
#define SPI_H

void	spi_init(u8 sercom);
void	spi_putc(unsigned char c);
void	spi_puts(u8 *str);
void	spi_put8(u8 hex);
void	spi_put16(u16 hex);

#endif

