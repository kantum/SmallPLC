#ifndef CAN_H
#define CAN_H

#include "types.h"
#include "mcp2515.h"
#include "color.h"
#include "error.h"

#define CLK		8000000                         /* Clock Frequency            */
#define TQ		0.0000005                       /* Time Quantum               */
#define BRP		(u8)(TQ * CLK) / 2 - 1          /* Baud Rate Prescaler Bits   */
#define SJW		0                               /* Synchronization Jump Width */
#define PROP	2                               /* PropSeg Size               */
#define PRSEG	PROP - 1                        /* PRSEG Register Value       */
#define BTL		1                               /* PS2 Bit Time Length Bit    */
#define SAM		0                               /* Sample Point Config Bit    */
#define SOF		0                               /* Start-Of-Frame Signal Bit  */
#define WAKFIL	0                               /* Wake-Up Filter Bit         */
#define PS1		2                               /* PS1 Size                   */
#define PHSEG1	PS1 - 1                         /* PHSEG1 Register Value      */
#define PS2		3                               /* PS2 Size                   */
#define PHSEG2	PS2 - 1                         /* PHSEG2 Register Value      */

/**
 * @brief CAN data structure
 */
typedef struct				s_can_msg 
{
	u32	id;                            /* Can Identifier           11/29 bits */
	u8	data[8];
	u8	len;			               /* Can Data                     8 bits */
	u8	prio:2;                        /* Can Priority                 2 bits */
	u8	ide:1;                         /* Extended Identifier          1 bit  */
	u8	rtr:1;                         /* Remote Transmission Request  1 bit  */
	u8	srr:1;                         /* Substitute Remote  Request   1 bit  */
} __attribute__ ((packed))	t_can_msg;

void	can_init();
void	can_tx_id(t_can_msg *msg);
void	can_baud_rate();
void	can_rtr_dlc(t_can_msg *msg);
void	can_send(t_can_msg *msg);
void	can_rx_id(t_can_msg *msg, u8 *buf);
void	can_receive(t_can_msg *msg, u8 buff);
void	can_set_msg(t_can_msg *msg, u32 id, u8 prio, u8 rtr, u8 len, u8 *data);

#endif
/* EOF */
