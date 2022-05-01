#ifndef _USART_H_
#define _USART_H_

#include "stdio.h"	
#include "sys.h" 



#define USART_DEBUG		USART1		//调试打印所使用的串口组


void Usart1_Init(unsigned int baud);

void Usart2_Init(unsigned int baud);

//void Usart3_Init(unsigned int baud);
//	
//void Usart4_Init(unsigned int baud);


void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);
void USART2_Send_data(char *str);
void USART2_Send_data_value(u8 dat);

#endif
