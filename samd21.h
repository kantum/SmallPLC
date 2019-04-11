#ifndef SAMD21_H
# define SAMD21_H

#include "types.h"
#include "hardware.h"

#define PM_ADDR				((u32)0x40000400)
#define SYSCTRL_ADDR		((u32)0x40000800)
#define PORTA_ADDR			((u32)0x41004400)
#define PORTB_ADDR			((u32)0x41004480)
#define UART_ADDR			((u32)0x42000800)
#define SPI_ADDR			((u32)0x42001c00)

/* PORT Registers */
#define P_DIR				0x00
#define P_DIRCLR			0x04
#define P_DIRSET			0x08
#define P_DIRTGL			0x0c
#define P_OUT				0x10
#define P_OUTCLR			0x14
#define P_OUTSET			0x18
#define P_OUTTGL			0x1c
#define P_IN				0x20
#define P_CTRL				0x24
#define P_WRCONFIG			0x28
#define P_PMUX				0x30
#define P_PINCFG			0x40
#define P_PINCFG31			0x5f

/* Power Manager Registers */
#define PM_CTRL				0x00
#define PM_SLEEP			0x01
#define PM_CPUSEL			0x08
#define PM_APBASEL			0x09
#define PM_APBBSEL			0x0A
#define PM_APBCSEL			0x0B
#define PM_AHBMASK			0x14
#define PM_APBAMASK			0x18
#define PM_APBBMASK			0x1c
#define PM_APBCMASK			0x20
#define PM_INTENCLR			0x34
#define PM_INTENSET			0x35
#define PM_INTFLAG			0x36
#define PM_RCAUSE			0x38

/* System Controler Registers */
#define SYSCTRL_INTENCLR	0x00
#define SYSCTRL_INTENSET	0x04
#define SYSCTRL_INTFLAG		0x08
#define SYSCTRL_PCLKSR		0x0c
#define SYSCTRL_XOSC		0x10
#define SYSCTRL_XOSC32K		0x14
#define SYSCTRL_OSC32K		0x18
#define SYSCTRL_OSCULP32K	0x1c
#define SYSCTRL_OSC8M		0x20
#define SYSCTRL_DFLLCTRL	0x24

/* Generic Clock Controller Registers */
#define	GCLK_CTRL			0x00
#define	GCLK_STATUS			0x01
#define	GCLK_CLKCTRL		0x02
#define	GCLK_GENCTRL		0x04
#define	GCLK_GENDIV			0x08

/* SERCOM USART */
#define USART_CTRLA			0x00
#define USART_CTRLB			0x04
#define USART_BAUD			0x0c
#define USART_RXPL			0x0e
#define USART_INTENCLR		0x14
#define USART_INTENSET		0x16
#define USART_INTFLAG		0x18
#define USART_STATUS		0x1A
#define USART_SYNCBUSY		0x1c
#define USART_DATA			0x28
#define USART_DBGCTRL		0x30
#define USART_FIFOSPACE		0x34
#define USART_FIFOPTR		0x36

/* SERCOM SPI */
#define SPI_CTRLA			0x00
#define SPI_CTRLB			0x04
#define SPI_BAUD			0x0c
#define SPI_RXPL			0x0e
#define SPI_INTENCLR		0x14
#define SPI_INTENSET		0x16
#define SPI_INTFLAG			0x18
#define SPI_STATUS			0x1A
#define SPI_SYNCBUSY		0x1c
#define SPI_DATA			0x28
#define SPI_DBGCTRL			0x30
#define SPI_FIFOSPACE		0x34
#define SPI_FIFOPTR			0x36

/* External Interrupt Controller */
#define EIC_CTRL			0x00
#define EIC_STATUS			0x01
#define EIC_NMICTRL			0x02
#define EIC_NMIFLAG			0x03
#define EIC_EVCTRL			0x04
#define EIC_INTENCLR		0x08
#define EIC_INTENSET		0x0C
#define EIC_INTFLAG			0x10
#define EIC_WAKEUP			0x14
#define EIC_CONFIG			0x18

void	sercom_init(u8 n, u8 clk);
void	interrupt_init(u8 clk);

#endif
