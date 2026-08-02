#ifndef __UART2_HMI_BLUETOOTH_H
#define __UART2_HMI_BLUETOOTH_H

extern uint8_t mode;
extern uint8_t A, B, C, D;

void UART2_HMI_BlueTooth_Init(uint32_t baudRate, uint8_t PreemptionPriority, uint8_t SubPriority);
void UART2_HMI_BlueTooth_SendByte(uint8_t Byte);
uint8_t UART2_GetRxFlag(void);

#endif
