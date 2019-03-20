#ifndef CAN_H
#define CAN_H

#include "types.h"
#include "mcp2515.h"
#include "color.h"

/**
 * @brief CAN data structure
 */
typedef struct	s_can_msg
{
	u32	id;                            /* Can identifier              11 bits */
	u8	ide;                           /* Extended identifier          1 bit  */
	u8	prio;                          /* Can priority                 2 bits */
	u8	rtr;                           /* Remote Transmission Request  1 bit  */
	u8	srr;                           /* Substitute Remote  Request   1 bit  */
	u8	len;			               /* Can data                     8 bits */
	u8	data[8];
}				t_can_msg;

void	can_init();
void	can_tx_id(u32 id);
void	can_baud_rate();
void	can_rtr_dlc(t_can_msg *msg);
void	can_send(t_can_msg *msg);
void	can_rx_id(t_can_msg *msg);
void	can_receive(t_can_msg *msg);

#endif
