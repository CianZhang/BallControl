#include "stm32f10x.h"                  		// Device header
#include "Delay.h"
#include "key.h"
#include "OLED.h"
#include "ball_control.h"

int main(void)
{
	OLED_Init();								//OLED初始化
	Key_Init();									//按键初始化
	Servo_Init();								//舵机初始化
	ball_control_Init();						//小球控制初始化
	
	UART3_OpenMV_Init(115200, 0, 1);			//OpenMV初始化
	UART2_HMI_BlueTooth_Init(115200, 0, 0);		//串口屏通过蓝牙的初始化
	UART1_VOFA_Init(115200, 0, 3);				//上位机通信初始化
	
	Servo_move_by_degress(75, 55);				//初始化角度
	
	while(1)
	{	
		ball_control();		
	}
}
