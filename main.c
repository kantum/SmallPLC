#include "types.h"
#include "hardware.h"
#include "samd21.h"
#include "uart.h"
#include "spi.h"
#include "mcp2515.h"

/**
 * @brief Initialize status leds
 * @section Led handling
 */
void	leds_init()
{
	/* Forget bootloader configuration for PB10 */
	reg8_wr((PORTB_ADDR + P_PINCFG + 10), 0);
	/* Configure PB10 and PB11 as output (DIR) */
	reg_wr(PORTB_ADDR + P_DIRSET, 1 << 10 | 1 << 11);
	/* Turn them off*/
	reg_wr(PORTB_ADDR + P_OUTCLR, 1 << 10 | 1 << 11);
}

typedef struct
{
	u32			id;
	struct
	{
		u8	rtr : 1;
		u8	length : 4;
	}			header;

	u8 data[8];
}can_msg;

void	can_tx_id(u16 sid)
{
	sid = sid & 0x7ff;
	can_wr_reg(TXB0SIDH, sid >> 3);       /* Identifier High */
	can_wr_reg(TXB0SIDL, sid & 0x7);      /* Identifier Low */
}

/**
 * @brief Don't you know main function ? Very handy one...
 */
int		main(void)
{
	leds_init();
	clock_init();
	uart_init(0);                  /* UART init on sercom0 */
	spi_init(5);                   /*  SPI init on sercom5 */

	if (can_reset() != 0)
		uart_debug("\e[33m[Warning]\e[0m failed, try to replug the module)");
	uart_debug("can_reset");
	//for (u16 i = 0; i < 254; ++i)
	//	can_wr_reg(i, 0);
	dump_memory(USABLE);
	can_wr_reg((TXB0CTRL | CANCTRL), 0b000 << 5);  /* CAN Normal Operation mode */
	uart_debug("(TXB0CTRL | CANCTRL), 0b000 << 5");
	can_wr_reg(TXB0CTRL, 0b01);       /* Priority */
	uart_debug("TXB0CTRL, 0b01");
	dump_memory(USABLE);

	can_tx_id(0xffff);

	can_wr_reg(TXB0DLC, 0x2);         /* 1 in DLC (Data Length Code) */
	uart_debug("TXB0DLC, 0x1");
	dump_memory(USABLE);

	can_wr_reg(CANINTE, 1 << 2);      /* Transmit Buffer 0 Empty Interrupt Enable bit */
	uart_debug("CANINTE, 1 << 2");
	dump_memory(USABLE);

	can_wr_reg(TXB0D0, 0x42);
	uart_debug("TXB0D0, 0x42");
	dump_memory(USABLE);

	can_rts(TXB0);
	uart_debug("can_rts(TXB0);");
	dump_memory(USABLE);

	while (can_rd_reg(CANINTF) == 0)
		;
	uart_debug("while (can_rd_reg(CANINTF) == 0)");
	dump_memory(USABLE);

	while (1)
	{
	}
}
/* EOF */
