#include "AD.h"

uint16_t AD_WaveBuf[BUFFER_SIZE];
uint8_t AD_Half_Flag = 0;
uint8_t AD_Full_Flag = 0;

void AD_Init(void)
{
    /* 1. 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 12MHz ADC时钟
    
    /* 2. GPIO 初始化 (PA0) */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 3. TIM3 初始化 (10us 触发一次 = 100kHz) */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 72MHz / 72 = 1MHz (1us 步长)
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; 
    // 1us * 10 = 10us (如果你发现数据丢包严重，可以把这里改为 20-1 降低到 20us)
    TIM_TimeBaseStructure.TIM_Period = 20 - 1; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 设置 TIM3 的 Update 事件作为 TRGO 输出，用来触发 ADC
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update); 
    
    /* 4. ADC 初始化 */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);
    
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    // 【关键】由 TIM3 TRGO 触发，而不是软件触发
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; 
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 由定时器脉冲决定转换，关闭连续模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    // 【关键修复：加上这一句，允许 ADC 接收 TIM3 的外部触发信号！】
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    /* 5. DMA1_Channel1 初始化 (ADC -> 内存) */
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_WaveBuf;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环乒乓模式
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
    /* 6. 配置 DMA 中断 (用于通知 CPU 什么时候发送数据) */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 开启传输过半(HT)和传输完成(TC)中断
    DMA_ITConfig(DMA1_Channel1, DMA_IT_HT | DMA_IT_TC, ENABLE); 
    
    /* 7. 使能外设 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET);
    
    // 启动定时器，ADC 就会以极其精准的 10us 间隔开始自动采样！
    TIM_Cmd(TIM3, ENABLE); 
}

// DMA1 Channel1 中断服务函数
void DMA1_Channel1_IRQHandler(void)
{
    // 上半场填满 (0 ~ 999 已经准备好)
    if (DMA_GetITStatus(DMA1_IT_HT1) != RESET) 
    {
        AD_Half_Flag = 1; 
        DMA_ClearITPendingBit(DMA1_IT_HT1);
    }
    // 下半场填满 (1000 ~ 1999 已经准备好)
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET) 
    {
        AD_Full_Flag = 1;
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}
