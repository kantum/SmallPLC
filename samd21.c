#include "samd21.h"

/**
 * @brief Initialize and configure SERCOM module
 * @param n SERCOM number
 * @param clk Chose wich clock to use GCLKGEN number
 */
void	sercom_init(u8 n, u8 clk)
{
	reg_set(PM_ADDR + PM_APBCMASK, 1 << (n + 2));
	reg16_wr (GCLK_ADDR + GCLK_CLKCTRL, (1 << 14)           /* Clock enable   */
			| (clk << 8)                                    /* GCLKGEN[clk]   */
			| 20 + n);                                      /* SERCOM[n]_CORE */
}

