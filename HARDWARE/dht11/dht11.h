#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
#include <stm32f10x.h>
//Set GPIO Direction
#define DHT11_IO_IN()  {GPIOC->CRL&=0XFFFFFFF0;GPIOC->CRL|=8<<0;}	//PA0<Important:�˴���&=����д��=�Ҿ�����һ����û�ҳ���>
#define DHT11_IO_OUT() {GPIOC->CRL&=0XFFFFFFF0;GPIOC->CRL|=3<<0;}	//PA0<ͬ��>
											   
#define	DHT11_DQ_OUT PCout(0) 
#define	DHT11_DQ_IN  PCin(0)   


u8 DHT11_Init(void); //Init DHT11
u8 DHT11_Read_Data(void); //Read DHT11 Value
u8 DHT11_Read_Byte(void);//Read One Byte
u8 DHT11_Read_Bit(void);//Read One Bit
u8 DHT11_Check(void);//Chack DHT11
void DHT11_Rst(void);//Reset DHT11    
#endif


