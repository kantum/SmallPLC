#ifndef TRANSFER_H
#define TRANSFER_H
#include "uart.h"
#include "can.h"

void	uart_to_can(t_can_msg *msg, u8 *buffer);
void	str_to_can(t_can_msg *msg, u8 *str);
u32		atoi_hex(const char *str);
int		isspace(int c);
int		ishexa(int c);
#endif
/* EOF */
