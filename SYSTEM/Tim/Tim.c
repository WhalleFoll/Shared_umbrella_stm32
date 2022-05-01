#include "Tim.h"
#include "sys.h"


extern unsigned char temp_flag,ping_flag;

void Tim1_Init(int arr,int psc)//500ms����һ��
{ 
     TIM_TimeBaseInitTypeDef TIM_Structure;               //���嶨ʱ���ṹ�����
	 NVIC_InitTypeDef NVIC_TIM;                           //�����ж�Ƕ�׽ṹ�����
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  //�򿪶�ʱ��ʱ��
	 
	 TIM_Structure.TIM_Period = (arr-1) ;         //�����Զ���װ�ؼĴ�������ֵ  ���ʱ��TimeOut= (arr)*(psc)/Tic    ��λΪus
     TIM_Structure.TIM_Prescaler = (psc-1);       //����Ԥ��Ƶֵ     
     TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up ;     //����ģʽ ��������
	 TIM_Structure.TIM_ClockDivision = TIM_CKD_DIV1;     //ʱ�ӷ�Ƶ      Tic=72M/��TIM_ClockDivision+1��
	 TIM_Structure.TIM_RepetitionCounter = 0; //�ظ������Ĵ���
	
	 TIM_TimeBaseInit(TIM1,&TIM_Structure);   //��ʼ����ʱ��1
	
	 NVIC_TIM.NVIC_IRQChannel = TIM1_UP_IRQn;  //��ʱ��1�����ϼ���ͨ��
	 NVIC_TIM.NVIC_IRQChannelCmd = ENABLE ;    //ʹ��
	 NVIC_TIM.NVIC_IRQChannelPreemptionPriority = 0 ;    //��ռ���ȼ�
	 NVIC_TIM.NVIC_IRQChannelSubPriority = 0;            //��Ӧ���ȼ� 
	
	 NVIC_Init(&NVIC_TIM);                     //��ʼ���ṹ��
	 
	 TIM_ClearFlag(TIM1,TIM_FLAG_Update);      //������б�־λ  ��֤����״̬��ʼ�� 
	 
	 TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);  //�򿪼�ʱ��
	
	 TIM_Cmd(TIM1,ENABLE);      	  	 	       //��TIM1
	
	 
}	

/*****��ʱ��1�ж���Ӧ����*****/
/*   ��Ҫ��ѯ�жϺ������� ��� startup_stm32f10x_hd.s�����ļ���ѯ��Ӧ���ж���Ӧ����*/
/*   ˵����TIM_ClearFlag()����������ö�ʱ�������б�־λ  
          һ����ʱ���ı�־λ�����ܶ��� TIM_IT_Update TIM_IT_CC1 TIM_IT_CC2 TIM_IT_CC3 �� 
          ���������þ�����ձ�־λ����TIM_ClearITPendingBit() ��ĳ����־λ������� �����û�м�����־λͬʱ���� ��������ʹ��Ч��Ӧ����һ����  */

void TIM1_UP_IRQHandler (void)                
{
	static unsigned int temp_count,ping_count;

	if(TIM_GetITStatus(TIM1,TIM_IT_Update) != RESET)   //����жϱ�־����1 ֤�����ж�
	{
	 
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);    // ��ձ�־λ��Ϊ��һ�ν����ж���׼��

	    temp_count++;
	    ping_count++;
		
		 if(temp_count == 2000)//10s
		 {
			 temp_flag=1;
			 temp_count=0;
		 } 
		if(ping_count == 8000)//40s
		 {
			 ping_flag=1;
			 ping_count=0;
		 } 
	}
  
}



void pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;   //����һ���ṹ�������������ʼ��GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//����һ���ṹ�������������ʼ����ʱ��

	TIM_OCInitTypeDef TIM_OCInitStructure;//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	/* ����ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;// PC6
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	//TIM3��ʱ����ʼ��
	TIM_TimeBaseInitStructure.TIM_Period = 199; //PWM Ƶ��=72000/(199+1)=36Khz//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7199;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//�ı�ָ���ܽŵ�ӳ��	//pC6

	//PWM��ʼ��	  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;

	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	//ע��˴���ʼ��ʱTIM_OC1Init������TIM_OCInit������������Ϊ�̼���İ汾��һ����
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܻ���ʧ��TIMx����
}


//void Tim2_Init(int arr,int psc)
//{ 
//   TIM_TimeBaseInitTypeDef TIM_Structure;               //���嶨ʱ���ṹ�����
//	 NVIC_InitTypeDef NVIC_TIM;                           //�����ж�Ƕ�׽ṹ�����
//	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  //�򿪶�ʱ��2ʱ��
//	
//	 TIM_Structure.TIM_Period = (arr-1) ;         //�����Զ���װ�ؼĴ�������ֵ  ���ʱ��TimeOut= (arr+1)*(psc+1)/Tic    
//   TIM_Structure.TIM_Prescaler = (psc-1);       //����Ԥ��Ƶֵ     
//   TIM_Structure.TIM_CounterMode = TIM_CounterMode_Up ;     //����ģʽ ��������
//	 TIM_Structure.TIM_ClockDivision = TIM_CKD_DIV1;     //ʱ�ӷ�Ƶ      Tic=72M/��TIM_ClockDivision+1��
//	 TIM_Structure.TIM_RepetitionCounter = 0; //�ظ������Ĵ���
//	
//	 TIM_TimeBaseInit(TIM2,&TIM_Structure);   //��ʼ����ʱ��1
//	
//	 NVIC_TIM.NVIC_IRQChannel = TIM2_IRQn;  //��ʱ��1�����ϼ���ͨ��
//	 NVIC_TIM.NVIC_IRQChannelCmd = ENABLE ;    //ʹ��
//	 NVIC_TIM.NVIC_IRQChannelPreemptionPriority = 0 ;    //��ռ���ȼ�
//	 NVIC_TIM.NVIC_IRQChannelSubPriority = 0;            //��Ӧ���ȼ� 
//	
//	 NVIC_Init(&NVIC_TIM);                     //��ʼ���ṹ��
//	 
//	 TIM_ClearFlag(TIM2,TIM_FLAG_Update);      //������б�־λ  ��֤����״̬��ʼ�� 
//	 
//	 TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);  //�򿪼�ʱ��
//	
//	 TIM_Cmd(TIM2,ENABLE);      	  	 	       //��TIM2
//	
//	 
//}	


//void TIM2_IRQHandler (void)                
//{
//           		 

//}
//  

