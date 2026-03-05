#include "stm32f10x.h"
#include "AD.h"
#include "Serial.h"

// 定义一个 8 位的发送数组
uint8_t Send_Buf_8bit[BUFFER_SIZE / 2]; 

// 压缩函数：将 16 位数组压缩成 8 位数组
void Convert_To_8Bit(uint16_t* raw_data, uint8_t* out_data, uint16_t len)
{
    for(int i = 0; i < len; i++)
    {
        // 原始数据 0~4095，右移 4 位后变成 0~255
        out_data[i] = (uint8_t)(raw_data[i] >> 4);
    }
}

int main(void)
{
    Serial_Init();
    AD_Init(); 
    
    while (1)
    {
        if (AD_Half_Flag == 1)
        {
            AD_Half_Flag = 0;
            // 压缩前半段
            Convert_To_8Bit(&AD_WaveBuf[0], Send_Buf_8bit, BUFFER_SIZE / 2);
            // 调用新的 8 位发送函数
            Serial_SendDMA_8bit(Send_Buf_8bit, BUFFER_SIZE / 2); 
        }
        
        if (AD_Full_Flag == 1)
        {
            AD_Full_Flag = 0;
            // 压缩后半段
            Convert_To_8Bit(&AD_WaveBuf[BUFFER_SIZE / 2], Send_Buf_8bit, BUFFER_SIZE / 2);
            // 调用新的 8 位发送函数
            Serial_SendDMA_8bit(Send_Buf_8bit, BUFFER_SIZE / 2);
        }
    }
}
