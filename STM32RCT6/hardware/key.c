#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t function_running_num;
uint8_t Flag_key = 0;					//按键标志位，确保按键按下只识别一次

void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		//开启GPIOC的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);						//将PC0 PC1引脚初始化为上拉输入
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;											//定义变量，默认键码值为0
	
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) == 0)			//读PC0输入寄存器的状态，如果为0，则代表按键1按下
	{
		KeyNum = 1;												//置键码为1
	}
	
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == 0)			//读PC1输入寄存器的状态，如果为0，则代表按键2按下
	{
		KeyNum = 2;												//置键码为2
	}
	
	return KeyNum;												//返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}

void read_key(void)
{
	uint8_t KEY = Key_GetNum();
	
		if(KEY == 1 && Flag_key == 0)
	{
		if(KEY == 1)
		{
			function_running_num = 1;
			Flag_key =	1;	
		}
	}
	
	if(KEY == 2 && Flag_key == 0)
	{
		if(KEY == 2)
		{
			function_running_num = 2;
			Flag_key =	1;
		}			
	}
	
	if(KEY == 0 && Flag_key == 1)								//按键标志位复位，确保只识别一次
	{								
		if(KEY == 0)
		{
			Flag_key = 0;
		}
    }
}
