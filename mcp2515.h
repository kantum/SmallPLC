#ifndef MCP2515_h
#define MCP2515_h

#define RESET	0xc0   /* 0b11000000 */
#define READ	0x03   /* 0b00000011 */
#define RDRX	0x90   /* 0b10010000 */
#define WRITE	0x02   /* 0b00000010 */
#define LDTX	0x40   /* 0b01000000 */
#define RTS		0x80   /* 0b10000000 */
#define RDSTA	0xA0   /* 0b10100000 */
#define RXSTA	0xb0   /* 0b10110000 */
#define BITM	0x05   /* 0b00000101 */

#endif
