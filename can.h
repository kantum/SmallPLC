#ifndef CAN_H
#define CAN_H

#include "types.h"
#include "mcp2515.h"
#include "color.h"

#define CLK		8000000                         /* Clock Frequency            */
#define TQ		0.000001                        /* Time Quantum               */
#define BRP		(u8)(TQ * CLK) / 2 - 1          /* Baud Rate Prescaler Bits   */
#define SJW		0                               /* Synchronization Jump Width */
#define PROP	2                               /* PropSeg Size               */
#define PRSEG	PROP - 1                        /* PRSEG Register Value       */
#define BTL		1                               /* PS2 Bit Time Length Bit    */
#define SAM		0                               /* Sample Point Config Bit    */
#define SOF		0                               /* Start-Of-Frame Signal Bit  */
#define WAKFIL	0                               /* Wake-Up Filter Bit         */
#define PS1		3                               /* PS1 Size                   */
#define PHSEG1	PS1 - 1                         /* PHSEG1 Register Value      */
#define PS2		4                               /* PS2 Size                   */
#define PHSEG2	PS2 - 1                         /* PHSEG2 Register Value      */

/**
 * @brief CAN data structure
 */
typedef struct	s_can_msg
{
	u32	id;                            /* Can Identifier              11 bits */
	u8	ide;                           /* Extended Identifier          1 bit  */
	u8	prio;                          /* Can Priority                 2 bits */
	u8	rtr;                           /* Remote Transmission Request  1 bit  */
	u8	srr;                           /* Substitute Remote  Request   1 bit  */
	u8	len;			               /* Can Data                     8 bits */
	u8	data[8];
}				t_can_msg;

void	can_init();
void	can_tx_id(u32 id);
void	can_baud_rate();
void	can_rtr_dlc(t_can_msg *msg);
void	can_send(t_can_msg *msg);
void	can_rx_id(t_can_msg *msg);
void	can_receive(t_can_msg *msg);
void	can_set_msg(t_can_msg *msg, u16 id, u8 prio, u8 rtr, u8 len, u8 *data);

#endif
/* EOF */
