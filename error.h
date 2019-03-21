#include "uart.h"
#include "mcp2515.h"

#ifndef error_h
#define error_h

#define STAT_RX0IF	1 << 0
#define STAT_RX1IF	1 << 1
#define STAT_TX0REQ	1 << 2
#define STAT_TX0IF	1 << 3
#define STAT_TX1REQ	1 << 4
#define STAT_TX1IF	1 << 5
#define STAT_TX2REQ	1 << 6
#define STAT_TX2IF	1 << 7

#define TEC			0x1C
#define REC			0x1D

void	can_get_error();

#endif
