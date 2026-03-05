#ifndef __AD_H
#define __AD_H

#include "stm32f10x.h"

// 定义乒乓缓冲区大小，2000 个数据点（4000 字节）
// 每次半满（1000个点）触发一次中断，也就是每 10ms 统一发送一次数据包
#define BUFFER_SIZE 2000 

extern uint16_t AD_WaveBuf[BUFFER_SIZE];
extern uint8_t AD_Half_Flag; // 缓冲区上半场填满标志
extern uint8_t AD_Full_Flag; // 缓冲区下半场填满标志

void AD_Init(void);

#endif

