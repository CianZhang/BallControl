#ifndef BALL_CONTROL_H
#define BALL_CONTROL_H

#include "stm32f10x.h"                  // Device header
#include "Servo.h"
#include "Timer.h"
#include "UART3_OpenMV.h"
#include "UART1_VOFA.h"
#include "UART2_HMI_BlueTooth.h"

extern uint8_t times;

typedef struct {
    int x;
    int y;
} Point;

void ball_control_Init(void);
void ball_control(void);

#endif
