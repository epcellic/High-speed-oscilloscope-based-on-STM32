#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"

void Serial_Init(void);
void Serial_SendDMA(uint16_t *data, uint16_t count);
void Serial_SendDMA_8bit(uint8_t *data, uint16_t count);

#endif
