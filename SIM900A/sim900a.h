#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
#include "usart.h"

void USART2_Init_SIM900A(u32 bound);
void USART2_Send_data(char *str);
void USART2_Send_data_value(u8 dat);
void borrow_mi_e(void);
void Recall_umbrella(void);
void gsm_borrow_Recall(u8 tel,u8 news);
#endif





