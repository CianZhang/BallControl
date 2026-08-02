#include "UART3_OpenMV.h"
#include <stdio.h>
#include <stdarg.h>

//OpenMV发送给STM32的数据的最大数量（一个代表一个字节）
#define OpenMV_data 10

//定义接收到的数据，设置为全局变量
//数据整合处理示例，将大于256的数据，由两个字节整合为实际值
//		uint16_t X1 = (UART3_receive_data[1] << 8) | UART3_receive_data[0];
//		points_x = (float)X1;

float points_x, points_y;						 //小球的实时位置坐标
int dx, dy;										 //小球的坐标差值


/*******************************************************************************************************/
uint16_t UART3_receive_data[OpenMV_data];        //接收的数据
uint8_t UART3_RxFlag;                   		 //定义接收数据包标志位

/**
 * @brief  USART3 初始化
 *         PB10->TX，PB11->RX
 * @param  baudRate 波特率
 *         PreemptionPriority 抢占优先级
 *         SubPriority 响应优先级
 * @retval None
 */
void UART3_OpenMV_Init(uint32_t baudRate, uint8_t PreemptionPriority, uint8_t SubPriority)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    USART_InitTypeDef USART_InitStructure ;
    USART_InitStructure.USART_BaudRate= baudRate;
    USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3,&USART_InitStructure);
 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_Init(&NVIC_InitStructure);
	
    USART_Cmd(USART3,ENABLE);
}

/**
 * @brief  串口发送一个字节
 * @param  Byte 要发送的一个字节
 * @retval None
 */
void UART3_OpenMV_SendByte(uint8_t Byte)
{
    USART_SendData(USART3, Byte);        									//将字节数据写入数据寄存器，写入后USART自动生成时序波形
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);    		//等待发送完成
    /*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
 * @brief  获取串口接收数据包标志位
 * @param  Byte 要发送的一个字节
 * @retval 串口接收数据包标志位，范围：0~1，接收到数据包后，标志位置1，读取后标志位自动清零
 */
uint8_t UART3_GetRxFlag(void)
{
    if (UART3_RxFlag == 1)            	//如果标志位为1
    {
        UART3_RxFlag = 0;
        return 1;                    	//则返回1，并自动清零标志位
    }
    return 0;                        	//如果标志位为0，则返回0
}

/**
 * @brief  USART3 中断函数
 * @param  None
 * @retval None
 */
void USART3_IRQHandler(void)
{
    static uint8_t RxState = 0;        								//定义表示当前状态机状态的静态变量
    static uint8_t pRxPacket = 0;      								//定义表示当前接收数据位置的静态变量
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)        	//判断是否是USART3的接收事件触发的中断
    {
        uint8_t RxData = USART_ReceiveData(USART3);                 //读取数据寄存器，存放在接收的数据变量
        
        /*使用状态机的思路，依次处理数据包的不同部分*/
        
        /*当前状态为0，接收数据包包头1*/
        if (RxState == 0)
        {
            if (RxData == 0xAA)            							//如果数据确实是包头
            {
                RxState = 1;               							//置下一个状态
            }
        }
        /*当前状态为1，接收数据包包头2*/
        else if (RxState == 1)
        {    
            if (RxData == 0xAE)
            {
                RxState = 2;
                pRxPacket = 0;
            }
        }
        /*当前状态为2，接收数据包数据*/
        else if (RxState == 2)
        {
            UART3_receive_data[pRxPacket] = RxData;        			//将数据存入数据包数组的指定位置
            pRxPacket ++;                            				//数据包的位置自增	
			
			if (pRxPacket == 11 && RxData == 0xAC)    				//收到的数据
			{
				uint16_t X1 = (UART3_receive_data[1] << 8) | UART3_receive_data[0];
				points_x = (float)X1;
				uint16_t Y1 = (UART3_receive_data[3] << 8) | UART3_receive_data[2];
				points_y = (float)Y1;

				dx = (int)UART3_receive_data[4];
				dy = (int)UART3_receive_data[6];
				
				if(UART3_receive_data[8] == 0xB2)
				{
					dx = -dx;
				}
				if(UART3_receive_data[9] == 0xC2)
				{
					dy = -dy;
				}
				
				RxState = 3;                        				//置下一个状态
			}
        }
		/*当前状态为3，接收数据包尾包*/
        else if(RxState == 3)
        {
            if (RxData == 0xAC)                     				//如果数据确实是包尾部
            {
                RxState = 0;                        				//回归状态0
                UART3_RxFlag = 1;                    				//接收数据包标志位置1，成功接收一个数据包    
            }
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);				//清除标志位
    }
}
