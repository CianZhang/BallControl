#ifndef _UART3_OPENMV_H
#define _UART3_OPENMV_H

#include "stm32f10x.h"

extern float points_x, points_y;
extern int dx, dy;

void UART3_OpenMV_Init(uint32_t baudRate, uint8_t PreemptionPriority, uint8_t SubPriority);
void UART3_OpenMV_SendByte(uint8_t Byte);

#endif
