#include "JiDianQi.h"

void JiDainQi_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_15|GPIO_Pin_13;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOG, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOG,GPIO_Pin_15); 						 //PG15输出高 
 GPIO_SetBits(GPIOG,GPIO_Pin_13); 						 //PG15输出高
 GPIO_SetBits(GPIOG,GPIO_Pin_11); 						 //PG15输出高 
 GPIO_SetBits(GPIOG,GPIO_Pin_9); 						 //PG15输出高
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

