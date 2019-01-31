/**
 * @file  main.c
 * @brief Main function of the firmware
 *
 */

/**
 * @brief Entry point
 *
 */
#include <stdint.h>
#include "types.h"

#define	PORTA		0x41004400
#define	PORTB		0x41004480

#define	DIR			0x00
#define	DIRCLR		0x04
#define	DIRSET		0x08
#define	DIRTGL		0x0c
#define	OUT			0x10
#define	OUTCLR		0x14
#define	OUTSET		0x18
#define	OUTTGL		0x1c
#define	IN			0x20
#define	CTRL		0x24
#define	WRCONFIG	0x28
#define	PMUX0		0x30
#define	PMUX15		0x3f
#define	PINCFG0		0x40
#define	PINCFG31	0x5f

#define	GCLK			0x30000c00 

#define	GCLK_CTRL		0x30000c00
#define	GCLK_STATUS		0x30000c01
#define	GCLK_CLKCTRL	0x30000c02
#define	GCLK_GENCTRL	0x30000c04
#define	GCLK_GENDIV		0x30000c08

#define	GENEN			(1<<16)
#define	OE				(1<<19)

void	init_clock(void)
{
	unsigned long		*reg16;
	unsigned char		*reg8;

	reg16 = (unsigned long *)GCLK_CLKCTRL;
	*reg16 = (1<<14);
	*reg16 |= (1<<14);
	reg16 = (unsigned long *)GCLK_GENCTRL;
	*reg16 = (GENEN | OE);

	/* Configure PA10 */
	reg8 = (unsigned char *)(PORTA + PINCFG0 + 10);
	*reg8 = (0x0);
	reg16 = (unsigned long *)(PORTA + DIR);
	*reg16 |= (1<<10); 
	reg8 = (unsigned char *)(PORTA + PINCFG0 + 10);
	*reg8 |= (0x1);
	reg8 = (unsigned char *)(PORTA + PMUX0 + 10);
	*reg8 |= (1<<7);
}

int main(void)
{
	unsigned long *reg64;
	unsigned char *reg8;

	/* Configure Pin Configuration */
	reg8  = (unsigned char *)(PORTB + PINCFG0 + 10);
	*reg8 = (0);

	/* Configure PB10 (DIR) */
	reg64  = (unsigned long *)(PORTB + DIR);
	*reg64 = (1<<10);

	/* Set PB10 value (OUT) */
	reg64  = (unsigned long *)(PORTB + OUT);
	*reg64 = (1<<10);

	/* Configure PB11 (DIR) */
	reg64  = (unsigned long *)(PORTB + DIR);
	*reg64 |= (1<<11);

	/* Set PB11 value (OUT) */
	reg64  = (unsigned long *)(PORTB + OUT);
	*reg64 |= (1<<11);

	//init_clock();

	/* Try to configure PA04 */
	reg64 = (unsigned long *)(PORTA + DIR);
	*reg64 = (1<<4);

	reg64  = (unsigned long *)(PORTA + OUTCLR);
	*reg64 = (1<<4);

	/* Configure PA10 to be clock output */
	    
	while (1);

}
/* EOF */
