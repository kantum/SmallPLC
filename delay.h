#ifndef DELAY_H
#define DELAY_H

#include "types.h"
#include "hardware.h"
#include "samd21.h"

void	delay_init(void);
void	delay(u32 ms);

#endif
