#include "JiDianQi.h"

void JiDainQi_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_15|GPIO_Pin_13;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOG, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOG,GPIO_Pin_15); 						 //PG15����� 
 GPIO_SetBits(GPIOG,GPIO_Pin_13); 						 //PG15�����
 GPIO_SetBits(GPIOG,GPIO_Pin_11); 						 //PG15����� 
 GPIO_SetBits(GPIOG,GPIO_Pin_9); 						 //PG15�����
}

void jidian_p(u8 i)
{
	switch(i)
	{
		case 4: {JD1=1;JD2=1;JD3=1;JD4=1;}break;
		case 3: {JD1=0;JD2=1;JD3=1;JD4=1;}break;
		case 2: {JD1=0;JD2=0;JD3=1;JD4=1;}break;
		case 1: {JD1=0;JD2=0;JD3=0;JD4=1;}break;
		case 0: {JD1=0;JD2=0;JD3=0;JD4=0;}break;
		default:break;		
	}

}

