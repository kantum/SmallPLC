#include "samd21.h"

/**
 * @brief Initialize and configure SERCOM module
 * @param n SERCOM number
 * @param clk Chose wich clock to use GCLKGEN number
 */
void	sercom_init(u8 n, u8 clk)
{
	reg_set(PM_ADDR + PM_APBCMASK, 1 << (n + 2));
	reg16_wr (GCLK_ADDR + GCLK_CLKCTRL, (1 << 14)           /* Clock Enable   */
			| (clk << 8)                                    /* GCLKGEN[clk]   */
			| 20 + n);                                      /* SERCOM[n]_CORE */
}

void	interrupt_init(u8 clk)
{
	u32 val;
	u8  pmux_val;

	reg_set(PM_ADDR + PM_APBAMASK, 1 << 6);         /* APBA Clock For EIC     */
	reg16_wr (GCLK_ADDR + GCLK_CLKCTRL, (1 << 14)   /* Clock Enable           */
			| (clk << 8)                            /* GCLKGEN[clk]           */
			| 0x05);                                /* GCLK_EIC               */
	/* Configure EIC filter for EXTINT2 */
	val = reg_rd(EIC_ADDR + EIC_CONFIG);            /* Read CONFIG            */
	val &= 0xFFFFF0FF;
	val |= (1 << 11) | (0x2 << 8);                  /* Falling-Edge Detect    */
	reg_wr(EIC_ADDR + EIC_CONFIG, val);             /* Write CONFIG           */
	reg_wr(EIC_ADDR + EIC_INTENCLR, (1 << 2));      /* Disable EXTINT2        */
	/* Enable EIC interrupt into NVIC */
	reg_wr(0xE000E100, (1 << 4));                   /* Enable EIC Itself      */
	reg_set(EIC_ADDR + EIC_CTRL, (1 << 1));
	reg8_wr((PORTA_ADDR + P_PINCFG + 2), 0x01);     /* PA02 INT               */
	reg8_wr((PORTA_ADDR + P_PMUX + 1), (0x0));      /* PA02 Pmux Function A   */
	reg_wr(EIC_ADDR + EIC_INTENSET, (1 << 2));      /* Enable EXTINT2 for EIC */
}
