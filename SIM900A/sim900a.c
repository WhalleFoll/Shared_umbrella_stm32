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
	
	//Usart2 NVIC 配置
 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_InitStructer.USART_BaudRate = bound;
	USART_InitStructer.USART_WordLength = 		 USART_WordLength_8b;
	USART_InitStructer.USART_StopBits = USART_StopBits_1;																		//设置停止位
	USART_InitStructer.USART_Parity = USART_Parity_No ;																			//设置校检位
	USART_InitStructer.USART_HardwareFlowControl = USART_HardwareFlowControl_None;					//关闭硬件控制模式
	USART_InitStructer.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;													//使能
	USART_Init(USART2,&USART_InitStructer); 
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);	
}


void USART2_Send_data(char *str)//用于发送字符串（GSM模块需要接受的命令）
{
	while(*str != '\0')
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
		USART_SendData(USART2,*str);
		str++;
	}
}


void USART2_Send_data_value(u8 dat)//用于发送十六进制数
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
//	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
//	va_end(ap);
//	
//	while(*pStr != 0)
//	{
//		USART_SendData(USARTx, *pStr++);
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
//	}

//}


//USART2_Send_data("AT+CMGF=1\r\n");	//短信格式设置为文本模式
//delay_ms(2000);//这里的延时可以更改但不能低于500MS如果太快模块接收不了
//USART2_Send_data("AT+CMGS=\"17104945750\"\r\n");//短信接受号码
//delay_ms(2000);
//USART2_Send_data("hello_word\r\n");//发送短信内容
//delay_ms(5000);
//USART2_Send_data_value(0x1a);

void borrow_mi_e(void)
{

	USART2_Send_data("AT+CMGF=1\r\n");	//短信格式设置为文本模式
	delay_ms(1000);//这里的延时可以更改但不能低于500MS如果太快模块接收不了
	USART2_Send_data("AT+CSCS=\"GSM\"\r\n");//设置编码格式为 T+CSCS="GSM"
	delay_ms(1000);
	USART2_Send_data("AT+CSCA?\r\n");//自动获取短信中心号码
	delay_ms(1000);
	USART2_Send_data("AT+CSMP=17,167,0,241\r\n");//
	delay_ms(1000);
	
	USART2_Send_data("AT+CMGS=\"17104945750\"\r\n");//短信接受号码
	delay_ms(1000);
	USART2_Send_data("verify: 123458 \r\n");//发送短信内容
	delay_ms(5000);
	USART2_Send_data_value(0x1a);

}


void Recall_umbrella(void)   //催促还伞
{
	
	USART2_Send_data("AT\r\n");//建立连接
	delay_ms(1000);
//	USART2_Send_data("AT\r\n");//建立连接
//	delay_ms(500);             //延时是必须的太快了模块接受不了。（笔者试了一下500MS是可以的如果不行增加延时即可）
	USART2_Send_data("AT+CMGF=1\r\n");//设置短信消息格式为文本模式
	delay_ms(1500);
	USART2_Send_data("AT+CSMP=17,167,2,25\r\n");//
	delay_ms(1500);
	USART2_Send_data("AT+CSCA?\r\n");//自动获取短信中心号码
	delay_ms(1500);
	USART2_Send_data("AT+CSCS=\"UCS2\"\r\n");//设置编码格式为USC2
	delay_ms(1500);
	USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");//接受短信号码，在接受号码的每一个数字前加上003
	//USART2_Send_data("AT+CMGS=\"%s\"\r\n",str);
	//UsartPrintf(USART2,"AT+CMGS=\"%s\"\r\n",str);
	delay_ms(1000);
	USART2_Send_data("8BF753CA65F68FD84F1E\r\n");//短信：请及时还伞
	delay_ms(5000);//这里最好延时5秒不然会出现发送短信不成功的现象
	USART2_Send_data_value(0x1a);//发送十六进制数0x1a表示发送内容结束
	
}
	

void gsm_borrow_Recall(u8 tel,u8 news)
{
 	USART2_Send_data("AT\r\n");//建立连接
	delay_ms(1000);
//	USART2_Send_data("AT\r\n");//建立连接
//	delay_ms(500);             //延时是必须的太快了模块接受不了。（笔者试了一下500MS是可以的如果不行增加延时即可）
	USART2_Send_data("AT+CMGF=1\r\n");//设置短信消息格式为文本模式
	delay_ms(1500);
	USART2_Send_data("AT+CSMP=17,167,2,25\r\n");//
	delay_ms(1500);
	USART2_Send_data("AT+CSCA?\r\n");//自动获取短信中心号码
	delay_ms(1500);
	USART2_Send_data("AT+CSCS=\"UCS2\"\r\n");//设置编码格式为USC2
	delay_ms(1500);
	switch(tel)
	{
		 case 1:USART2_Send_data("AT+CMGS=\"00310035003900350039003300340036003900310035\"\r\n");break;  // 15959346915
         case 2:USART2_Send_data("AT+CMGS=\"00310035003000390033003700380036003300380031\"\r\n");break;  // 15093786381
         case 3:USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");break;
         case 4:USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");break;
         default:break;		
	
	}
		
	//USART2_Send_data("AT+CMGS=\"00310037003100300034003900340035003700350030\"\r\n");//接受短信号码，在接受号码的每一个数字前加上003
	//USART2_Send_data("AT+CMGS=\"%s\"\r\n",str);
	//UsartPrintf(USART2,"AT+CMGS=\"%s\"\r\n",str);
	delay_ms(1000);
    switch(news)
	{
		 case 1:USART2_Send_data("8BF753CA65F68FD84F1E\r\n");break;                              //短信内容;及时还伞
		 case 2:USART2_Send_data("53D64F1E9A8C8BC17801FF1A003100320033003400350036\r\n");break;  //取伞验证码:123456
         case 3:USART2_Send_data("53D64F1E9A8C8BC17801003A003500340033003100310032\r\n");break; //取伞验证码:543112
         case 4:USART2_Send_data("53D64F1E9A8C8BC17801003A003500340033003200310031\r\n");break; //取伞验证码:543211		
	     default:break;	
	}
	//USART2_Send_data("8BF753CA65F68FD84F1E\r\n");   //短信内容
	delay_ms(5000);//这里最好延时5秒不然会出现发送短信不成功的现象
	USART2_Send_data_value(0x1a);//发送十六进制数0x1a表示发送内容结束
}


void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		
		//RxData2=USART_ReceiveData(USART2);
		
		// ress= USART_ReceiveData(USART2); 	 
		// USART_SendData(USART1,res);
		 USART_ClearFlag(USART2, USART_FLAG_RXNE);

	}

}



