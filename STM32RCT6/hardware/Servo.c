#include "stm32f10x.h"                  			// Device header
#include "PWM.H"
#include "Delay.h"
#include "PID.h"
#include "UART3_OpenMV.h"

uint8_t servo_rotation_limit[2] = {10,150};			//旋转轴舵机的限位值
uint8_t servo_pitch_limit[2] = {10, 150};			//仰俯轴舵机的限位值
uint8_t servo_rotation_direction = 0;				//旋转轴舵机的方向，数值增加，光点向右为1，反之为0
uint8_t servo_pitch_direction = 0;					//仰俯轴舵机的方向，数值增加，光点向下为1，反之为0
float servo_rotation_value = 75;					//旋转轴舵机的初始值
float servo_pitch_value = 55;						//仰俯轴舵机的初始值

PID_TypeDef p_pid_x;
PID_TypeDef p_pid_y;
PID_TypeDef v_pid_x;
PID_TypeDef v_pid_y;

void Servo_Init(void)
{
	PWM_Init();
	
	PID_Init(&p_pid_x, 0.1, 0.0016, 0.08);
    PID_Init(&p_pid_y, 0.1, 0.00255, 0.08);
	
	PID_Init(&v_pid_x, 0, 0, 1.1);
    PID_Init(&v_pid_y, 0, 0, 1.1);
}

void Servo_move_by_degress(float rotation, float pitch)
{
	if (rotation > servo_rotation_limit[1])
	{
		rotation = servo_rotation_limit[1];
	}
	if (rotation < servo_rotation_limit[0]) 
	{
		rotation = servo_rotation_limit[0];
	}
	if (pitch > servo_pitch_limit[1]) 
	{
		pitch = servo_pitch_limit[1];
	}
	if (pitch < servo_pitch_limit[0]) 
	{
		pitch = servo_pitch_limit[0];
	}
	
	PWM_SetCompare1(rotation / 180 * 2000  + 500);	//这个就对应了从从角度0-180度映射到占空比500-2500从而进行驱动舵机
	PWM_SetCompare2(pitch / 180 * 2000  + 500);		//这个就对应了从从角度0-180度映射到占空比500-2500从而进行驱动舵机
}

void Servo_move_increment(float x, float y)
{
    if(servo_rotation_direction == 0)				//根据设置，是否调转x轴方向
	{
        x = -x;
	}
    if(servo_pitch_direction == 0)					//根据设置，是否调转y轴方向
	{
        y = -y;
	}
	
	servo_rotation_value = servo_rotation_value + x;
	servo_pitch_value = servo_pitch_value + y;
	
	Servo_move_by_degress(servo_rotation_value, servo_pitch_value);
}

void Servo_move_to_traget_close_loop(float target_x, float target_y)
{
	PID_SetSetpoint(&p_pid_x, target_x); 			
    PID_SetSetpoint(&p_pid_y, target_y);
	
	float control_signal_x = PID_Update(&p_pid_x, points_x);
    float control_signal_y = PID_Update(&p_pid_y, points_y);
	
	PID_SetSetpoint(&v_pid_x, control_signal_x); 			
	PID_SetSetpoint(&v_pid_y, control_signal_y);
	
	//PID_SetSetpoint(&v_pid_x, 0); 			
    //PID_SetSetpoint(&v_pid_y, 0);
	
	float control_x = PID_Update(&v_pid_x, dx);
    float control_y = PID_Update(&v_pid_y, dy);
		
	Servo_move_increment(control_x, control_y);
}
