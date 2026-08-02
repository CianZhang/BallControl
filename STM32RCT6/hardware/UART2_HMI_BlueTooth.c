#include "stm32f10x.h"                  		// Device header
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

//蓝牙发送给STM32的数据的最大数量（一个代表一个字节）
#define BlueTooth_data 5

//定义接收到的数据，设置为全局变量

uint8_t mode;
uint8_t A, B, C, D;


/*******************************************************************************************************/
uint16_t UART2_receive_data[BlueTooth_data]; 	//接收的数据
uint8_t UART2_RxFlag;                   		//定义接收数据包标志位

/**
 * @brief  USART2 串口屏通过蓝牙的初始化
 *         PA2->TX，PA3->RX
 * @param  baudRate 波特率
 *         PreemptionPriority 抢占优先级
 *         SubPriority 响应优先级
 * @retval None
 */
void UART2_HMI_BlueTooth_Init(uint32_t baudRate, uint8_t PreemptionPriority, uint8_t SubPriority)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure ;
	USART_InitStructure.USART_BaudRate= baudRate;
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd =	ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2,ENABLE);
}

/**
 * @brief  串口发送一个字节
 * @param  Byte 要发送的一个字节
 * @retval None
 */
void UART2_HMI_BlueTooth_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
 * @brief  获取串口接收数据包标志位
 * @param  None
 * @retval 串口接收数据包标志位，范围：0~1，接收到数据包后，标志位置1，读取后标志位自动清零
 */
uint8_t UART2_GetRxFlag(void)
{
    if (UART2_RxFlag == 1)            	//如果标志位为1
    {
        UART2_RxFlag = 0;
        return 1;                    	//则返回1，并自动清零标志位
    }
    return 0;                        	//如果标志位为0，则返回0
}


/**
 * @brief  USART2 中断接收数据函数
 * @param  None
 * @retval None
 */
void USART2_IRQHandler(void)
{
    static uint8_t RxState = 0;        								//定义表示当前状态机状态的静态变量
    static uint8_t pRxPacket = 0;      								//定义表示当前接收数据位置的静态变量
	
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)        	//判断是否是USART2的接收事件触发的中断
    {
        uint8_t RxData = USART_ReceiveData(USART2);                 //读取数据寄存器，存放在接收的数据变量
        
        /*使用状态机的思路，依次处理数据包的不同部分*/
        
        /*当前状态为0，接收数据包包头*/
        if (RxState == 0)
        {
            if (RxData == 0x55)            								//如果数据确实是包头
            {
                RxState = 1;               								//置下一个状态
            }
        }
		/*当前状态为1，接收数据包数据*/
        else if (RxState == 1)
        {
            UART2_receive_data[pRxPacket] = RxData;        				//将数据存入数据包数组的指定位置
            pRxPacket ++;                            					//数据包的位置自增	
			
			if (pRxPacket == 2 && RxData == 0x0D)    					//收到的数据
			{
				if ((UART2_receive_data[0] & 0xF0) == 0x00) 			//检查接收到的字节是否符合0x0X的格式
				{
					uint8_t digit = UART2_receive_data[0] & 0x0F;
					if (digit >= 1 && digit <= 9) 
					{
						mode = digit;
						digit = 0;
						pRxPacket = 0;
					}
				}
					
				RxState = 2;                        					//置下一个状态
			}
			
			if (pRxPacket == 6 && RxData == 0x0D)    					//收到的数据
			{					
				if ((UART2_receive_data[1] & 0xF0) == 0x30)				//检查接收到的字节是否符合0x3X的格式
				{
					uint8_t digit = UART2_receive_data[1] & 0x0F;
					if (digit >= 1 && digit <= 9) 
					{
						A = digit;
					}
				}
				if ((UART2_receive_data[2] & 0xF0) == 0x30)				// 检查接收到的字节是否符合0x3X的格式
				{
					uint8_t digit = UART2_receive_data[2] & 0x0F;
					if (digit >= 1 && digit <= 9) 
					{
						B = digit;
					}
				}
				if ((UART2_receive_data[3] & 0xF0) == 0x30)				// 检查接收到的字节是否符合0x3X的格式
				{
					uint8_t digit = UART2_receive_data[3] & 0x0F;
					if (digit >= 1 && digit <= 9) 
					{
						C = digit;
					}
				}
				if ((UART2_receive_data[4] & 0xF0) == 0x30)				// 检查接收到的字节是否符合0x3X的格式
				{
					uint8_t digit = UART2_receive_data[4] & 0x0F;
					if (digit >= 1 && digit <= 9) 
					{
						D = digit;
					}
				}
				mode = 6;
				
				pRxPacket = 0;
		
				RxState = 2;                        					//置下一个状态
			}
        }
		/*当前状态为2，接收数据包包尾1*/
		else if(RxState == 2)
		{
			if(RxData == 0x0D)
			{
				RxState = 3;
			}
		}
		/*当前状态为3，接收数据包包尾2*/
		else if(RxState == 3)
		{
			if(RxData == 0x0A)
			{
				RxState = 0;
				UART2_RxFlag = 1;                    					//接收数据包标志位置1，成功接收一个数据包   
			}
		}
    
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);					//清除标志位
    }
}
