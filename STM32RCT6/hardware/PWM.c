#include "stm32f10x.h"                  // Device header

/**函数：pwm初始化
  *参数：PA0 
  */
void PWM_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);			//开启TIM2的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);		//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	TIM_InternalClockConfig(TIM2);
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;					//定义结构体变量
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 20000-1;      				//计数周期，即ARR的值!!!!
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;     				//预分频器，即PSC的值!!!!
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);					//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元
	/*APB2时钟工作频率为72MHz,分频器是72，计数器周期是20000。产生了一个50 Hz的PWM信号。*/  
	
	/*输出比较初始化*/ 
	TIM_OCInitTypeDef TIM_OCInitStruct;								//定义结构体变量
	TIM_OCStructInit(&TIM_OCInitStruct);                            //结构体初始化，若结构体没有完整赋值
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;                  //输出比较模式，选择PWM模式1
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;          //输出极性，选择为高，若选择极性为低，则输出高低电平取反填充的值（初始化）
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;      //输出使能 
	TIM_OCInitStruct.TIM_Pulse = 0;       							//初始的CCR值别输出PWM
	TIM_OC1Init(TIM2,&TIM_OCInitStruct);						    //将结构体变量交给TIM_OC1Init，配置TIM2的输出比较通道1
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);							//将结构体变量交给TIM_OC2Init，配置TIM2的输出比较通道2
                                                                   
	/*TIM使能*/
	TIM_Cmd(TIM2,ENABLE);			//使能TIM2，定时器开始运行
}


/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~20000
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare1(uint16_t Compare1)
{
	TIM_SetCompare1(TIM2,Compare1);			//CCR1
}

//设置预填充值
void PWM_SetCompare2(uint16_t Compare2)
{
	TIM_SetCompare2(TIM2,Compare2);			//CCR2
}
