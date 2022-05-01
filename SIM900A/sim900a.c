#include "sim900a.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>




void USART2_Init_SIM900A(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructer;
	USART_InitTypeDef USART_InitStructer;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructer.GPIO_Pin = GPIO_Pin_2;//TX
	GPIO_InitStructer.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructer.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructer);
	
	GPIO_InitStructer.GPIO_Pin = GPIO_Pin_3;//RX
	GPIO_InitStructer.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_InitStructer.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructer);
	
	//Usart2 NVIC ����
 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_InitStructer.USART_BaudRate = bound;
	USART_InitStructer.USART_WordLength = 		 USART_WordLength_8b;
	USART_InitStructer.USART_StopBits = USART_StopBits_1;																		//����ֹͣλ
	USART_InitStructer.USART_Parity = USART_Parity_No ;																			//����У��λ
	USART_InitStructer.USART_HardwareFlowControl = USART_HardwareFlowControl_None;					//�ر�Ӳ������ģʽ
	USART_InitStructer.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;													//ʹ��
	USART_Init(USART2,&USART_InitStructer); 
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);	
}


void USART2_Send_data(char *str)//���ڷ����ַ�����GSMģ����Ҫ���ܵ����
{
	while(*str != '\0')
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
		USART_SendData(USART2,*str);
		str++;
	}
}


void USART2_Send_data_value(u8 dat)//���ڷ���ʮ��������
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==0);
	USART_SendData(USART2,dat);
}


//void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
//{

//	unsigned char UsartPrintfBuf[296];
//	va_list ap;
//	unsigned char *pStr = UsartPrintfBuf;
//	
//	va_start(ap, fmt);
//	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
//	va_end(ap);
//	
//	while(*pStr != 0)
//	{
//		USART_SendData(USARTx, *pStr++);
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
//	}

//}


//USART2_Send_data("AT+CMGF=1\r\n");	//���Ÿ�ʽ����Ϊ�ı�ģʽ
//delay_ms(2000);//�������ʱ���Ը��ĵ����ܵ���500MS���̫��ģ����ղ���
//USART2_Send_data("AT+CMGS=\"17104945750\"\r\n");//���Ž��ܺ���
//delay_ms(2000);
//USART2_Send_data("hello_word\r\n");//���Ͷ�������
//delay_ms(5000);
//USART2_Send_data_value(0x1a);

void borrow_mi_e(void)
{

	USART2_Send_data("AT+CMGF=1\r\n");	//���Ÿ�ʽ����Ϊ�ı�ģʽ
	delay_ms(1000);//�������ʱ���Ը��ĵ����ܵ���500MS���̫��ģ����ղ���
	USART2_Send_data("AT+CSCS=\"GSM\"\r\n");//���ñ����ʽΪ T+CSCS="GSM"
	delay_ms(1000);
	USART2_Send_data("AT+CSCA?\r\n");//�Զ���ȡ�������ĺ���
	delay_ms(1000);
	USART2_Send_data("AT+CSMP=17,167,0,241\r\n");//
	delay_ms(1000);
	
	USART2_Send_data("AT+CMGS=\"17104945750\"\r\n");//���Ž��ܺ���
	delay_ms(1000);
	USART2_Send_data("verify: 123458 \r\n");//���Ͷ�������
	delay_ms(5000);
	USART2_Send_data_value(0x1a);

}


void Recall_umbrella(void)   //�ߴٻ�ɡ
{
	
	USART2_Send_data("AT\r\n");//��������
	delay_ms(1000);
//	USART2_Send_data("AT\r\n");//��������
//	delay_ms(500);             //��ʱ�Ǳ����̫����ģ����ܲ��ˡ�����������һ��500MS�ǿ��Ե��������������ʱ���ɣ�
	USART2_Send_data("AT+CMGF=1\r\n");//���ö�����Ϣ��ʽΪ�ı�ģʽ
	delay_ms(1500);
	USART2_Send_data("AT+CSMP=17,167,2,25\r\n");//
	delay_ms(1500);
	USART2_Send_data("AT+CSCA?\r\n");//�Զ���ȡ�������ĺ���
	delay_ms(1500);
	USART2_Send_data("AT+CSCS=\"UCS2\"\r\n");//���ñ����ʽΪUSC2
	delay_ms(1500);
	USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");//���ܶ��ź��룬�ڽ��ܺ����ÿһ������ǰ����003
	//USART2_Send_data("AT+CMGS=\"%s\"\r\n",str);
	//UsartPrintf(USART2,"AT+CMGS=\"%s\"\r\n",str);
	delay_ms(1000);
	USART2_Send_data("8BF753CA65F68FD84F1E\r\n");//���ţ��뼰ʱ��ɡ
	delay_ms(5000);//���������ʱ5�벻Ȼ����ַ��Ͷ��Ų��ɹ�������
	USART2_Send_data_value(0x1a);//����ʮ��������0x1a��ʾ�������ݽ���
	
}
	

void gsm_borrow_Recall(u8 tel,u8 news)
{
 	USART2_Send_data("AT\r\n");//��������
	delay_ms(1000);
//	USART2_Send_data("AT\r\n");//��������
//	delay_ms(500);             //��ʱ�Ǳ����̫����ģ����ܲ��ˡ�����������һ��500MS�ǿ��Ե��������������ʱ���ɣ�
	USART2_Send_data("AT+CMGF=1\r\n");//���ö�����Ϣ��ʽΪ�ı�ģʽ
	delay_ms(1500);
	USART2_Send_data("AT+CSMP=17,167,2,25\r\n");//
	delay_ms(1500);
	USART2_Send_data("AT+CSCA?\r\n");//�Զ���ȡ�������ĺ���
	delay_ms(1500);
	USART2_Send_data("AT+CSCS=\"UCS2\"\r\n");//���ñ����ʽΪUSC2
	delay_ms(1500);
	switch(tel)
	{
		 case 1:USART2_Send_data("AT+CMGS=\"00310035003900350039003300340036003900310035\"\r\n");break;  // 15959346915
         case 2:USART2_Send_data("AT+CMGS=\"00310035003000390033003700380036003300380031\"\r\n");break;  // 15093786381
         case 3:USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");break;
         case 4:USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");break;
         default:break;		
	
	}
		
	//USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");//���ܶ��ź��룬�ڽ��ܺ����ÿһ������ǰ����003
	//USART2_Send_data("AT+CMGS=\"%s\"\r\n",str);
	//UsartPrintf(USART2,"AT+CMGS=\"%s\"\r\n",str);
	delay_ms(1000);
    switch(news)
	{
		 case 1:USART2_Send_data("8BF753CA65F68FD84F1E\r\n");break;                              //��������;��ʱ��ɡ
		 case 2:USART2_Send_data("53D64F1E9A8C8BC17801FF1A003100320033003400350036\r\n");break;  //ȡɡ��֤��:123456
         case 3:USART2_Send_data("53D64F1E9A8C8BC17801003A003500340033003100310032\r\n");break; //ȡɡ��֤��:543112
         case 4:USART2_Send_data("53D64F1E9A8C8BC17801003A003500340033003200310031\r\n");break; //ȡɡ��֤��:543211		
	     default:break;	
	}
	//USART2_Send_data("8BF753CA65F68FD84F1E\r\n");   //��������
	delay_ms(5000);//���������ʱ5�벻Ȼ����ַ��Ͷ��Ų��ɹ�������
	USART2_Send_data_value(0x1a);//����ʮ��������0x1a��ʾ�������ݽ���
}


void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		
		//RxData2=USART_ReceiveData(USART2);
		
		// ress= USART_ReceiveData(USART2); 	 
		// USART_SendData(USART1,res);
		 USART_ClearFlag(USART2, USART_FLAG_RXNE);

	}

}



