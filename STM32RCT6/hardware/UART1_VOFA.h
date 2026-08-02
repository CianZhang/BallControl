#ifndef _UART1_VOFA_H
#define _UART1_VOFA_H

void UART1_VOFA_Init(uint32_t baudRate, uint8_t PreemptionPriority, uint8_t SubPriority);
void USART1_printf(char* fmt,...);

//float Get_Data(void);
//void USART1_PID_Adjust(uint8_t Servo_n);

#endif
