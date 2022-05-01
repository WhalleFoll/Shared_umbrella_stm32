//Ӳ������
#include "usart.h"
#include "delay.h"
#include "stm32f10x.h"
//C��
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


unsigned short data_e = 0;
u8 ress;
/*
************************************************************
*	�������ƣ�	Usart1_Init
*
*	�������ܣ�	����1��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA9		RX-PA10
************************************************************
*/

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 



void Usart1_Init(unsigned int baud)
{

	GPIO_InitTypeDef gpio_initstruct;
	USART_InitTypeDef usart_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//PA9	TXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_9;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_initstruct);
	
	//PA10	RXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_initstruct);
	
	usart_initstruct.USART_BaudRate = baud;
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usart_initstruct.USART_Parity = USART_Parity_No;									//��У��
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART1, &usart_initstruct);
	
	USART_Cmd(USART1, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvic_initstruct.NVIC_IRQChannel = USART1_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_initstruct);

}

/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA2		RX-PA3
************************************************************
*/
//void Usart2_Init(unsigned int baud)
//{

//	GPIO_InitTypeDef gpio_initstruct;
//	USART_InitTypeDef usart_initstruct;
//	NVIC_InitTypeDef nvic_initstruct;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	
//	//PA2	TXD
//	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	gpio_initstruct.GPIO_Pin = GPIO_Pin_2;
//	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &gpio_initstruct);
//	
//	//PA3	RXD
//	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	gpio_initstruct.GPIO_Pin = GPIO_Pin_3;
//	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &gpio_initstruct);
//	
//	usart_initstruct.USART_BaudRate = baud;
//	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
//	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
//	usart_initstruct.USART_Parity = USART_Parity_No;									//��У��
//	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
//	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
//	USART_Init(USART2, &usart_initstruct);
//	
//	USART_Cmd(USART2, ENABLE);														//ʹ�ܴ���
//	
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
//	
//	nvic_initstruct.NVIC_IRQChannel = USART2_IRQn;
//	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
//	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
//	nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init(&nvic_initstruct);

//}


//void USART2_Send_data(char *str)//���ڷ����ַ�����GSMģ����Ҫ���ܵ����
//{
//	while(*str != '\0')
//	{
//		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
//		USART_SendData(USART2,*str);
//		str++;
//	}
//}


//void USART2_Send_data_value(u8 dat)//���ڷ���ʮ��������
//{
//	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==0);
//	USART_SendData(USART2,dat);
//}


////USART3_RX	  PB.11
////USART3_TX   PB.10

//void Usart3_Init(unsigned int baud)
//{
//      GPIO_InitTypeDef gpio_initstruct;
//	  USART_InitTypeDef usart_initstruct;
//	  NVIC_InitTypeDef nvic_initstruct;
//	 
//	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��PORTBʱ��
//	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //ʹ��USART3
// 	  //USART_DeInit(USART3);  //��λ����3
//	  //USART3_TX   PB.10	
//	  gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	  gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
//	  gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
//	  GPIO_Init(GPIOB, &gpio_initstruct);
//	 
//	  //USART3_RX	  PB.11
//	  gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	  gpio_initstruct.GPIO_Pin = GPIO_Pin_11;
//	  gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
//	  GPIO_Init(GPIOB, &gpio_initstruct);
//    
//     
//	    usart_initstruct.USART_BaudRate = baud;
//	    usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
//		usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
//		usart_initstruct.USART_Parity = USART_Parity_No;									//��У��
//		usart_initstruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
//		usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
//		USART_Init(USART3, &usart_initstruct);
//		
//		USART_Cmd(USART3, ENABLE);														//ʹ�ܴ���
//		
//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
//		
//		nvic_initstruct.NVIC_IRQChannel = USART3_IRQn;
//		nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
//		nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 1;
//		nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
//		NVIC_Init(&nvic_initstruct);
//	  
//}


void Usart4_Init(unsigned int baud)
{
    GPIO_InitTypeDef gpio_initstruct;
	USART_InitTypeDef usart_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PORTCʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //ʹ��UART4
	
	//UART4_TX   PC.10
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio_initstruct);
	
	////UART4_RX	  PC.11
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_11;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio_initstruct);
	
	usart_initstruct.USART_BaudRate = baud;
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usart_initstruct.USART_Parity = USART_Parity_No;									//��У��
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(UART4, &usart_initstruct);
	
	USART_Cmd(UART4, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvic_initstruct.NVIC_IRQChannel = UART4_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&nvic_initstruct);
}
/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{

	unsigned char UsartPrintfBuf[296];
	va_list ap;
	unsigned char *pStr = UsartPrintfBuf;
	
	va_start(ap, fmt);
	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
	va_end(ap);
	
	while(*pStr != 0)
	{
		USART_SendData(USARTx, *pStr++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
	}

}

/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����1�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/



void USART1_IRQHandler(void)
{
	
     if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
	{
		
//		if(data_e >= sizeof(RxData2))	
//		data_e = 0; //��ֹ���ڱ�ˢ��
//		
//		//RxData1[data_e++]=USART_ReceiveData(USART1);
//		RxData1[data_e++] = USART1->DR;
//		
    		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);	

	}
}
	

//void USART2_IRQHandler(void)
//{

//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
//	{
//		
//		//RxData2=USART_ReceiveData(USART2);
//		
//		 ress= USART_ReceiveData(USART2); 	 
//		// USART_SendData(USART1,res);
//		 USART_ClearFlag(USART2, USART_FLAG_RXNE);

//	}

//}





