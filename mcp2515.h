#ifndef MCP2515_h
#define MCP2515_h

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

void	can_reset(void);
u8		can_rd_reg(u8 addr);
u8		can_rd_rx(u8 buff, u8 ptr);
void	can_wr_reg(u8 addr, u8 data);
void	can_wr_tx(u8 buff, u8 ptr, u8 data);
void	can_rts(u8 tx);
void	can_bit_mod(u8 addr, u8 mask, u8 data);
u8		can_rd_sta(void);
#endif
