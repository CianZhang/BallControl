#include "stm32f10x.h"                  // Device header
#include "PID.h"

#define integral_limit 600				//积分限幅值

// 初始化PID
void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = 0;
    pid->integral = 0;
    pid->previous_error = 0;
}

// 更新PID
float PID_Update(PID_TypeDef *pid, float feedback_value) {
    float error = pid->setpoint - feedback_value;
    pid->integral += error;
	
	if (pid->integral > integral_limit)           pid->integral = integral_limit;
    if (pid->integral < -integral_limit)          pid->integral = -integral_limit;
	
    float derivative = error - pid->previous_error;
    pid->previous_error = error;

    float output = (pid->Kp * error) + (pid->Ki * pid->integral) + (pid->Kd * derivative);
    return output;
}

// 设置目标值
void PID_SetSetpoint(PID_TypeDef *pid, float setpoint) {
    pid->setpoint = setpoint;
}

/*使用示例*/
//int main(void) {
//    // 系统初始化代码 (如时钟配置等)

//    // 创建两个PID实例
//    PID_TypeDef pid1;
//    PID_TypeDef pid2;

//    // 初始化两个PID实例，设定初始目标值（setpoint）
//    PID_Init(&pid1, 1.0, 0.1, 0.01, 100.0); // 第一个PID的初始目标值设为100.0
//    PID_Init(&pid2, 1.5, 0.2, 0.02, 150.0); // 第二个PID的初始目标值设为150.0

//    float feedback_value1 = 0.0;
//    float feedback_value2 = 0.0;

//    while (1) {
//        // 假设feedback_value1和feedback_value2从不同的传感器读取
//        // 这里可以插入传感器读取代码

//        // 动态改变目标值的示例
//        PID_SetSetpoint(&pid1, 120.0); // 改变第一个PID的目标值
//        PID_SetSetpoint(&pid2, 130.0); // 改变第二个PID的目标值

//        float control_signal1 = PID_Update(&pid1, feedback_value1);
//        float control_signal2 = PID_Update(&pid2, feedback_value2);

//        // 将control_signal1和control_signal2分别发送到不同的执行器，如PWM控制电机
//        // 这里可以插入执行器控制代码

//        // 模拟反馈值更新（实际应用中应由传感器反馈）
//        feedback_value1 += control_signal1 * 0.1; // 简单模拟系统响应
//        feedback_value2 += control_signal2 * 0.1; // 简单模拟系统响应
//    }
//}
