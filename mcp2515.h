#ifndef MCP2515_h
#define MCP2515_h

#include "types.h"
#include "spi.h"
#include "uart.h"
#include "samd21.h"
#include "hardware.h"
#include "delay.h"

#define RESET		0xc0   /* 0b11000000 */
#define READ		0x03   /* 0b00000011 */
#define RDRX		0x90   /* 0b10010000 */
#define WRITE		0x02   /* 0b00000010 */
#define LDTX		0x40   /* 0b01000000 */
#define RTS			0x80   /* 0b10000000 */
#define RDSTA		0xA0   /* 0b10100000 */
#define RXSTA		0xb0   /* 0b10110000 */
#define BITM		0x05   /* 0b00000101 */

#define BUFF0		0b00   /* Receive Buffer 0 */
#define BUFF1		0b01   /* Receive Buffer 1 */
#define BUFF2		0b10   /* Receive Buffer 1 */
#define RXBnSIDH	0b00   /* Start at RXBnSIDH */
#define RXBnD0		0b01   /* Start at RXBnD0 */

#define TXBnSIDH	0b00   /* Start at TXBnSIDH*/
#define TXBnD0		0b01   /* Start at TXBnD0 */

#define TXB0		(1 << 0)
#define TXB1		(1 << 1)
#define TXB2		(1 << 2)

#define RXF0		0x00
#define RXF0SIDH	0x00
#define RXF0SIDL	0x01
#define RXF0EID8	0x02
#define RXF0EID0	0x03
#define RXF1		0x04
#define RXF1SIDH	0x04
#define RXF1SIDL	0x05
#define RXF1EID8	0x06
#define RXF1EID0	0x07
#define RXF2		0x08
#define RXF2SIDH	0x08
#define RXF2SIDL	0x09
#define RXF2EID8	0x0A
#define RXF2EID0	0x0B
#define BFPCTRL		0x0C
#define TXRTSCTRL	0x0D
#define CANSTAT		0x0E
#define CANCTRL		0x0F

#define RXF3		0x10
#define RXF3SIDH	0x10
#define RXF3SIDL	0x11
#define RXF3EID8	0x12
#define RXF3EID0	0x13
#define RXF4		0x14
#define RXF4SIDH	0x14
#define RXF4SIDL	0x15
#define RXF4EID8	0x16
#define RXF4EID0	0x17
#define RXF5		0x18
#define RXF5SIDH	0x18
#define RXF5SIDL	0x19
#define RXF5EID8	0x1A
#define RXF5EID0	0x1B
#define TEC			0x1C
#define REC			0x1D

#define RXM0		0x20
#define RXM0SIDH	0x20
#define RXM0SIDL	0x21
#define RXM0EID8	0x22
#define RXM0EID0	0x23
#define RXM1		0x24
#define RXM1SIDH	0x24
#define RXM1SIDL	0x25
#define RXM1EID8	0x26
#define RXM1EID0	0x27
#define CNF3		0x28
#define CNF2		0x29
#define CNF1		0x2A
#define CANINTE		0x2B
#define CANINTF		0x2C
#define EFLG		0x2D

#define TXB0CTRL	0x30
#define TXB0SIDH	0x31
#define TXB0SIDL	0x32
#define TXB0EID8	0x33
#define TXB0EID0	0x34
#define TXB0DLC		0x35
#define TXB0D0		0x36
#define TXB0D1		0x37
#define TXB0D2		0x38
#define TXB0D3		0x39
#define TXB0D4		0x3A
#define TXB0D5		0x3B
#define TXB0D6		0x3C
#define TXB0D7		0x3D

#define TXB1CTRL	0x40
#define TXB1SIDH	0x41
#define TXB1SIDL	0x42
#define TXB1EID8	0x43
#define TXB1EID0	0x44
#define TXB1DLC		0x45
#define TXB1D0		0x46
#define TXB1D1		0x47
#define TXB1D2		0x48
#define TXB1D3		0x49
#define TXB1D4		0x4A
#define TXB1D5		0x4B
#define TXB1D6		0x4C
#define TXB1D7		0x4D

#define TXB2CTRL	0x50
#define TXB2SIDH	0x51
#define TXB2SIDL	0x52
#define TXB2EID8	0x53
#define TXB2EID0	0x54
#define TXB2DLC		0x55
#define TXB2D0		0x56
#define TXB2D1		0x57
#define TXB2D2		0x58
#define TXB2D3		0x59
#define TXB2D4		0x5A
#define TXB2D5		0x5B
#define TXB2D6		0x5C
#define TXB2D7		0x5D

#define RXB0CTRL	0x60
#define RXB0SIDH	0x61
#define RXB0SIDL	0x62
#define RXB0EID8	0x63
#define RXB0EID0	0x64
#define RXB0DLC		0x65
#define RXB0D0		0x66
#define RXB0D1		0x67
#define RXB0D2		0x68
#define RXB0D3		0x69
#define RXB0D4		0x6A
#define RXB0D5		0x6B
#define RXB0D6		0x6C
#define RXB0D7		0x6D

#define RXB1CTRL	0x70
#define RXB1SIDH	0x71
#define RXB1SIDL	0x72
#define RXB1EID8	0x73
#define RXB1EID0	0x74
#define RXB1DLC		0x75
#define RXB1D0		0x76
#define RXB1D1		0x77
#define RXB1D2		0x78
#define RXB1D3		0x79
#define RXB1D4		0x7A
#define RXB1D5		0x7B
#define RXB1D6		0x7C

#define USABLE		8
#define FULL		16

int		can_reset(void);
int		can_hw_reset(void);
void	can_rd_reg(u8 addr, u8 *data, u8 len);
u8		can_rd_rx(u8 buff, u8 ptr);
void	can_wr_reg(u8 addr, u8 *data, u8 len);
void	can_ld_tx(u8 buff, u8 ptr, u8 data);
void	can_rts(u8 tx);
void	can_bit_mod(u8 addr, u8 mask, u8 data);
u8		can_rd_sta(void);
u8		can_rx_sta(void);
void	dump_memory(u8 size);
#endif
