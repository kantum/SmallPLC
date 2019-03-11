#include "delay.h"

void	delay_init(void)
{
	reg_wr(GCLK_ADDR + GCLK_GENDIV, (1 << 8) | 0x02);
	/*                                  \        \____ GCLK2
	 *                                   \____________ No divisor */

	reg_wr(GCLK_ADDR + GCLK_GENCTRL, (1 << 16)    /* Enable */
			| (0x04 << 8)					      /* Source Select -> OSC32K */
			| 0x02);                              /* Select GCLK2 */
}

void	delay(u32 ms) /* TODO delay not precise at all TODO */
{
	ms <<= 15;
	for (u32 i = 0; i < (ms + 1); i++)
		asm("nop");
}
