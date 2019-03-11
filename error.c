#include "error.h"

#define TEC	0x1C
#define REC	0x1D

void	can_print_status()
{
	u8	status;

	status = can_rd_sta();
	uart_puts("status : ");
	uart_puthex8(status);
	uart_crlf();
	if (status & STAT_RX0IF)
		uart_debug("STAT_RX0IF");
	if (status & STAT_RX1IF)
		uart_debug("STAT_RX1IF");
	if (status & STAT_TX0REQ)
		uart_debug("STAT_TX0REQ");
	if (status & STAT_TX0IF)
		uart_debug("STAT_TX0IF");
	if (status & STAT_TX1REQ)
		uart_debug("STAT_TX1REQ");
	if (status & STAT_TX1IF)
		uart_debug("STAT_TX1IF");
	if (status & STAT_TX2REQ)
		uart_debug("STAT_TX2REQ");
	if (status & STAT_TX2IF)
		uart_debug("STAT_TX2IF");
	uart_crlf();
}

void can_print_tec()
{
	/* Transmit Error Counter */
	uart_puts("Transmit Error Counter : ");
	uart_puthex8(can_rd_reg(TEC));
	uart_crlf();
}

void can_print_rec()
{
	/* Receive  Error Counter */
	uart_puts("Receive  Error Counter : ");
	uart_puthex8(can_rd_reg(REC));
	uart_crlf();
}

#define RX1OVR	1 << 7
#define RX0OVR	1 << 6
#define TXBO	1 << 5
#define TXEP	1 << 4
#define RXEP	1 << 3
#define TXWAR	1 << 2
#define RXWAR	1 << 1
#define EWARN	1 << 0
void	can_print_eflg()
{
	u8	eflg;

	eflg = can_rd_reg(EFLG);
	uart_puts("EFLG : ");
	uart_puthex8(eflg);
	uart_crlf();
	if (eflg & RX1OVR)
		uart_debug("RX1OVR");
	if (eflg & RX0OVR)
		uart_debug("RX0OVR");
	if (eflg & TXBO)
		uart_debug("TXBO");
	if (eflg & TXEP)
		uart_debug("TXEP");
	if (eflg & RXEP)
		uart_debug("RXEP");
	if (eflg & TXWAR)
		uart_debug("TXWAR");
	if (eflg & RXWAR)
		uart_debug("RXWAR");
	if (eflg & EWARN)
		uart_debug("EWARN");
}

void	can_get_error()
{
	can_print_status();
	can_print_tec();
	can_print_rec();
	can_print_eflg();
}
