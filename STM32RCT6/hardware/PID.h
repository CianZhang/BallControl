#ifndef PID_H
#define PID_H

// PID结构体定义
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float setpoint;
    float integral;
    float previous_error;
} PID_TypeDef;

// 初始化PID函数声明
void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd);

// 更新PID函数声明
float PID_Update(PID_TypeDef *pid, float feedback_value);

// 设置目标值函数声明
void PID_SetSetpoint(PID_TypeDef *pid, float setpoint);

#endif
