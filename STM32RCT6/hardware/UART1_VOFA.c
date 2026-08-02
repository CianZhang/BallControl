#include "stm32f10x.h"                  // Device header
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define USART_TX_BUF_SIZE 1024

uint8_t USART_TX_BUF[USART_TX_BUF_SIZE];

//uint8_t RxBuffer[1];										//串口接收缓冲
//uint16_t RxLine = 0;										//指令长度
//uint8_t DataBuff[200];									//指令内容

/**
 * @brief  USART1 初始化
 *         PA9->TX，PA10->RX
 * @param  baudRate 波特率
 *         PreemptionPriority 抢占优先级
 *         SubPriority 响应优先级
 * @retval None
 */
void UART1_VOFA_Init(uint32_t baudRate, uint8_t PreemptionPriority, uint8_t SubPriority)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA9引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA10引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					//定义结构体变量
	USART_InitStructure.USART_BaudRate = baudRate;			//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		//奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长，选择8位
	USART_Init(USART1, &USART_InitStructure);				//将结构体变量交给USART_Init，配置USART1
	
	/*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//开启串口接收数据的中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			//配置NVIC为分组2
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;		//指定NVIC线路的抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;					//指定NVIC线路的响应优先级
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);								//使能USART1，串口开始运行
}

/**
 * @brief  USART1_printf函数重定义
 * @param  要发送的数据
 *         USART1_printf("%f,%f,%d,%d\n",points_x, points_y, 320, 97);
 * @retval None
 */
void USART1_printf(char* fmt,...)
{
    uint16_t i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)USART_TX_BUF, fmt, ap);  				// 使用vsprintf进行格式化输出
    va_end(ap);
    i = strlen((const char*)USART_TX_BUF);  				// 此次发送数据的长度
    for (j = 0; j < i; j++)  								// 循环发送数据
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  // 等待上次传输完成
        USART_SendData(USART1, (uint8_t)USART_TX_BUF[j]);  // 发送数据到串口1
    }
}

/*以下是通过串口调参，解析串口发送的数据*/
///*
// * 解析出DataBuff中的数据
// * 返回解析得到的数据
// */
//float Get_Data(void)
//{
//    uint8_t data_Start_Num = 0; // 记录数据位开始的地方
//    uint8_t data_End_Num = 0; // 记录数据位结束的地方
//    uint8_t data_Num = 0; // 记录数据位数
//    uint8_t minus_Flag = 0; // 判断是不是负数
//    float data_return = 0; // 解析得到的数据
//	
//    for(uint8_t i=0; i<200; i++) // 查找等号和感叹号的位置
//    {
//        if(DataBuff[i] == '=') data_Start_Num = i + 1; // +1是直接定位到数据起始位
//        if(DataBuff[i] == '!')
//        {
//            data_End_Num = i - 1;
//            break;
//        }
//    }
//	
//    if(DataBuff[data_Start_Num] == '-') // 如果是负数
//    {
//        data_Start_Num += 1; // 后移一位到数据位
//        minus_Flag = 1; // 负数flag
//    }
//	
//    data_Num = data_End_Num - data_Start_Num + 1;
//    if(data_Num == 4) // 数据共4位
//    {
//        data_return = (DataBuff[data_Start_Num] - '0') + 
//                      (DataBuff[data_Start_Num+2] - '0') * 0.1f +
//                      (DataBuff[data_Start_Num+3] - '0') * 0.01f;
//    }
//	
//    else if(data_Num == 5) // 数据共5位
//    {
//        data_return = (DataBuff[data_Start_Num] - '0') * 10 +
//                      (DataBuff[data_Start_Num+1] - '0') + 
//                      (DataBuff[data_Start_Num+3] - '0') * 0.1f +
//                      (DataBuff[data_Start_Num+4] - '0') * 0.01f;
//    }
//	
//    else if(data_Num == 6) // 数据共6位
//    {
//        data_return = (DataBuff[data_Start_Num] - '0') * 100 +
//                      (DataBuff[data_Start_Num+1] - '0') * 10 +
//                      (DataBuff[data_Start_Num+2] - '0') +
//                      (DataBuff[data_Start_Num+4] - '0') * 0.1f +
//                      (DataBuff[data_Start_Num+5] - '0') * 0.01f;
//    }
//	
//    if(minus_Flag == 1) data_return = -data_return;
//    // printf("data=%.2f\r\n", data_return);
//    return data_return;
//}
//
///*
// * 根据串口信息进行PID调参
// */
//void USART1_PID_Adjust(uint8_t Servo_n)
//{
//    float data_Get = Get_Data(); 					  // 存放接收到的数据
//	
//    if(Servo_n == 1)  								  // x轴舵机
//    {
//        if(DataBuff[0]=='P' && DataBuff[1]=='1') 	  // 位置环P
//            posPIDX.kp = data_Get;
//		
//        else if(DataBuff[0]=='I' && DataBuff[1]=='1') // 位置环I
//            posPIDX.ki = data_Get;
//		
//        else if(DataBuff[0]=='D' && DataBuff[1]=='1') // 位置环D
//            posPIDX.kd = data_Get;
//		
//        else if(DataBuff[0]=='P' && DataBuff[1]=='2') // 速度环P
//            velPIDX.kp = data_Get;
//		
//        else if(DataBuff[0]=='I' && DataBuff[1]=='2') // 速度环I
//            velPIDX.ki = data_Get;
//		
//        else if(DataBuff[0]=='D' && DataBuff[1]=='2') // 速度环D
//            velPIDX.kd = data_Get;
//		
//        else if((DataBuff[0]=='S' && DataBuff[1]=='p') && DataBuff[2]=='e') //目标速度
//            velPIDX.target = data_Get;
//		
//        else if((DataBuff[0]=='P' && DataBuff[1]=='o') && DataBuff[2]=='s') //目标位置
//            posPIDX.target = data_Get;
//    }
//	
//    else if(Servo_n == 2) 							  // y轴舵机
//    {
//        if(DataBuff[0]=='P' && DataBuff[1]=='1')      // 位置环P
//            posPIDY.kp = data_Get;
//		
//        else if(DataBuff[0]=='I' && DataBuff[1]=='1') // 位置环I
//            posPIDY.ki = data_Get;
//		
//        else if(DataBuff[0]=='D' && DataBuff[1]=='1') // 位置环D
//            posPIDY.kd = data_Get;
//		
//        else if(DataBuff[0]=='P' && DataBuff[1]=='2') // 速度环P
//            velPIDY.kp = data_Get;
//		
//        else if(DataBuff[0]=='I' && DataBuff[1]=='2') // 速度环I
//            velPIDY.ki = data_Get;
//		
//        else if(DataBuff[0]=='D' && DataBuff[1]=='2') // 速度环D
//            velPIDY.kd = data_Get;
//		
//        else if((DataBuff[0]=='S' && DataBuff[1]=='p') && DataBuff[2]=='e') //目标速度
//            velPIDY.target = data_Get;
//		
//        else if((DataBuff[0]=='P' && DataBuff[1]=='o') && DataBuff[2]=='s') //目标位置
//            posPIDY.target = data_Get;
//    }
//}
//
//void USART1_IRQHandler(void) {
//    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
//        // 读取接收到的数据
//        uint8_t data = USART_ReceiveData(USART1);

//        // 将数据存储到缓冲区
//        DataBuff[RxLine++] = data;

//        // 检查是否接收到特定结束标志（这里示例为0x21）
//        if (data == 0x21) 
//		{
//			// 打印接收到的数据长度
//            printf("RXLen=%d\r\n", RxLine);
//			
//			// 打印接收到的每个数据
//            for (int i = 0; i < RxLine; i++) {
//                printf("UART DataBuff[%d] = %c\r\n", i, DataBuff[i]);
//            }
//			
//			//x轴舵机为1，y轴舵机为2
//            USART1_PID_Adjust(1);
//            memset(DataBuff, 0, sizeof(DataBuff));
//            RxLine = 0;
//        }

//        // 再次使能中断（在标准库中通常不需要，因为中断会自动再使能，但确保USART_IT_RXNE始终使能）
//        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//    }
//}
