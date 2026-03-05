#include "Serial.h"

void Serial_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    // PA9 (TX)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    USART_InitTypeDef USART_InitStructure;
    // 【核弹级波特率】2,000,000！ CH340G 和 STM32 都能支持
    USART_InitStructure.USART_BaudRate = 2000000; 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx; // 只发不收
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);
    
    // 开启串口的 DMA 发送请求
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);
    
    // 初始化 DMA1_Channel4 (内存 -> 串口)
    DMA_InitTypeDef DMA_InitStructure_Tx;
    DMA_InitStructure_Tx.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure_Tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 串口是8位8位发的
    DMA_InitStructure_Tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure_Tx.DMA_MemoryBaseAddr = 0; // 等会儿发送时再动态分配
    DMA_InitStructure_Tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存也按8位读
    DMA_InitStructure_Tx.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure_Tx.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure_Tx.DMA_BufferSize = 0;
    DMA_InitStructure_Tx.DMA_Mode = DMA_Mode_Normal; // 单次发送
    DMA_InitStructure_Tx.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure_Tx.DMA_Priority = DMA_Priority_Medium;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure_Tx);
}

// 使用 DMA 高速把半个数组的数据倾泻到串口
void Serial_SendDMA(uint16_t *data, uint16_t count)
{
    DMA_Cmd(DMA1_Channel4, DISABLE); // 先关闭DMA
    
    // 【关键修复：每次重新发送前，清除一下上一轮发送完成的标志位】
    DMA_ClearFlag(DMA1_FLAG_TC4); 
    
    // 指向要发送的内存首地址
    DMA1_Channel4->CMAR = (uint32_t)data; 
    // count 是 16 位的数量，串口按 8 位发，所以长度要乘 2
    DMA_SetCurrDataCounter(DMA1_Channel4, count * 2); 
    
    DMA_Cmd(DMA1_Channel4, ENABLE); // 重新开启DMA发送
}
// 专门发送 8 位数据的 DMA 函数
void Serial_SendDMA_8bit(uint8_t *data, uint16_t count)
{
    DMA_Cmd(DMA1_Channel4, DISABLE); 
    DMA_ClearFlag(DMA1_FLAG_TC4); 
    
    DMA1_Channel4->CMAR = (uint32_t)data; 
    // 注意：这里不再乘 2，因为传进来的就是 8 位字节的数量
    DMA_SetCurrDataCounter(DMA1_Channel4, count); 
    
    DMA_Cmd(DMA1_Channel4, ENABLE); 
}
