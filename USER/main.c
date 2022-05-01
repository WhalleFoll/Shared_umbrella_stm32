#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "flash.h"
#include "sram.h"
#include "malloc.h"
#include "string.h"
#include "mmc_sd.h"
#include "ff.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "piclib.h"
#include "string.h"
#include "fattester.h"
#include "piclib.h"
//#include "usmart.h"
#include "rtc.h" 
#include "beep.h"
#include "timer.h"
#include "JiDianQi.h"

#include "rc522.h"
#include "24cxx.h"
#include "touch.h"
#include "sim900a.h"

#include <stdio.h>
#include <stdlib.h>
#define N 10      //员工


struct stu_info{
    int stunum;  /* 6位员工号  */
    char stutel[11]; /* 11位电话号 */
    int  tel_rank;   /* 员工排名   */
    int u_state;   /* 借伞状态   */
	u16 h_time; /* 还伞时间   */
	u16 j_time; /* 借伞时间   */
}stu[N]={  {392001,"15959346915",1,0,0,0},    //电话号 瞎几把写的
           {392002,"15093786381",2,0,0,0},
		   {392003,"17104945750",3,0,0,0},
		   {392004,"13133333333",4,0,0,0},
		   {392005,"13200000000",5,0,0,0},
		   {392006,"15111111111",6,0,0,0},
		   {392007,"17211444444",7,0,0,0},
		   {392008,"17355555555",8,0,0,0},
		   {392009,"18287777777",9,0,0,0},
		   {392010,"18388888888",10,0,0,0}
         };


//*******************************************************************************//
u8 RC522_ID[4];   //存放IC卡的类型
u8 RC522_NUM[4];  //存放IC卡的卡号
u8 RC522_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};  //存放密码
u8 RC522_return;  //返回的状态
u8 li;
u8 ptel=0;
u8 pter=0;
u8 num2[6];
u8 numl[6];
u8 ye=0;
int Ur=4; //伞
int n_tel;
int s_tel;
u8 duan_xin=0;
extern u16 gl_time;

u8 rfid_renz(void);
void ui_main(void);
void Caidan_ui(void);
void ShaXin_ui(void);
u8 ctp_test(void);
u8 ui_input_s(void);
u8 ui_input_tel(void);
void ui_input(void);
void panduan_tel(void);
void  staff_ui(void);
void  umbrella_ui(void); 
void huan_usan(void);
//u32 Shi=0;
//u32 Fen=0;
//u8  ding=0;
//u8  beering=0;

//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息	
	u8 *fn;	 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	return rval;
}
	

 int main(void)
 {	 
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO picfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	//u8 key2;				//键值2
//    u8 tou1=0;
//	u8 tou2=0;
	 


	//u8 pause=0;			//暂停标记
	//u8 t;
	u16 temp;
	u16 *picindextbl;	//图片索引表
	

 
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Usart1_Init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
	LCD_Init();	
	KEY_Init();	 	
	//usmart_dev.init(72);//usmart初始化	
 	mem_init(SRAMIN);	//初始化内部内存池	
 	exfuns_init();					//为fatfs相关变量申请内存  
  	f_mount(0,fs[0]); 		 		//挂载SD卡 
 	f_mount(1,fs[1]); 				//挂载FLASH.
	POINT_COLOR=BLACK;      
	RTC_Init();	  			//RTC初始化
	BEEP_Init();   //蜂鸣器初始化
	TIM2_Base_Init(10000);//10Khz的计数频率，计数到10000为1s 
	
	RC522_Init();  //  **RC522初始化*
	Pcd_Reset();   //   复位RC522
	M500PcdConfigISOType('A');  //设置工作方式
	JiDainQi_Init();  //jidianqi
	tp_dev.init();    //触摸屏
	
	USART2_Init_SIM900A(115200);  //SIM900A 初始化

	TIM_SetCompare1(TIM3, 185);//关
	while(RTC_Init())		//RTC初始化	，一定要初始化成功
	{ 
		delay_ms(500);
	}	
 	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
	}  	 
 	Show_Str(60,50,200,16,"智能卧室闹钟",16,0);				    	 
	Show_Str(60,70,200,16,"图片显示....",16,0);				    	 
	Show_Str(60,90,200,16,"S3:NEXT S1:PREV",16,0);				    	 
	Show_Str(60,110,200,16,"www.doflye.net",16,0);				    	 
	Show_Str(60,130,200,16,"SD卡需要拷贝图片",16,0);
	
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(60,150,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				 
		LCD_Fill(60,150,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(60,150,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,150,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//为带路径的文件名分配内存
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);				//申请2*totpicnum个字节的内存,用于存放图片索引
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//内存分配出错
 	{	    
		Show_Str(60,150,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,150,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.index;								//记录当前index
	        res=f_readdir(&picdir,&picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picindextbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	Show_Str(60,150,240,16,"开始显示...",16,0); 
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
  	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
	
        ui_main();
	
		//  主循环
		while(1) 
		{

			
			key=KEY_Scan(0);
			switch(key)
			{
				case 0:ShaXin_ui();break;  // 没有按键按下 一直刷新页面
				case 1:{                   // 按键一 按下 				    
                        ye=0;staff_ui();
					    while(1)
						{
						 key=KEY_Scan(0);
						 if(key>=3){ui_main(); break;}
						 else if(key==2 && ye==0){ye++;staff_ui();}
						 else if(key==1&& ye==1){ye--;staff_ui();}				  
						}
				       };break;
				case 2:{
                        LCD_Clear(WHITE);			
					    while(1)
						{							
						 key=rfid_renz();
						 delay_ms(200);
						 if(key==4) break;
                         else if(key==3 && stu[0].u_state==0 && Ur>0) //借
						 {							  
						   stu[0].u_state=1;
						   stu[0].j_time=gl_time;
                           Ur--;							 
						   LCD_Clear(WHITE); Show_Str(60,150,240,16,"取伞成功",16,0);delay_ms(1000);
						   break;
						 }
                         else if(key==2 && stu[0].u_state==1) //还
						 {							  
						   stu[0].u_state=0;
						   stu[0].j_time=0;
						   Ur++;
						   LCD_Clear(WHITE); Show_Str(60,150,240,16,"还伞成功",16,0);delay_ms(1000);
						   break;
						 }						 
						}
                        ui_main();						
				       };break;	
				case 3:{
						if(Ur>0)
						{	
							s_tel=0;panduan_tel();
							if(s_tel==123456 ||s_tel==543112)   //比对验证码
							{
								if(stu[duan_xin-1].u_state==0)
								{
								  LCD_Clear(WHITE);
								  Show_Str(60,150,240,16,"取伞成功...",16,0);delay_ms(1000);
								  stu[duan_xin-1].u_state=1;  //借伞
								  stu[duan_xin-1].j_time=gl_time; //借伞时间
								  Ur--;                       //可用伞减少
								  duan_xin=0;	
								}
								
							}
							else
							{
							   LCD_Clear(WHITE);
							   Show_Str(60,150,240,16,"取伞失败...",16,0);delay_ms(1000);
							}
							ui_main();
						}
                      else
						{
						   LCD_Clear(WHITE);
						   Show_Str(60,150,240,16,"没伞了！",16,0);delay_ms(1000);
						   ui_main();
						}	
  						  
				       };break;
				case 4: {
                         POINT_COLOR=BLUE;					
					     huan_usan();
					     if(duan_xin>0)
						 {
						 	if(stu[duan_xin-1].u_state==1)  //还伞
							{
								stu[duan_xin-1].u_state=0;
								Ur++;                       //可用伞++
								stu[duan_xin-1].j_time=0;
								LCD_Clear(WHITE);Show_Str(60,150,240,16,"还伞成功",16,0);delay_ms(1000);
							}
							else 
							{   
							 LCD_Clear(WHITE);Show_Str(60,150,240,16,"还伞失败",16,0);delay_ms(1000);
							}
						 }
					     ui_main();
				       };break;	

			}

			delay_ms(100); 
		//}					    
		res=0;  
	} 											  
//	myfree(SRAMIN,picfileinfo.lfname);	//释放内存			    
//	myfree(SRAMIN,pname);				//释放内存			    
//	myfree(SRAMIN,picindextbl);			//释放内存			    
}


u8 rfid_renz(void)
{
	    Show_Str(2,300,240,16,"         还伞    借伞   返回",16,0);
	    Show_Str(2,32,240,16,"   请将员工卡放置至感应区!",16,0);	
	    Show_Str(10,56,56,16,"卡类型:",16,0);
	    Show_Str(10,80,56,16,"卡  号:",16,0);
		Show_Str(10,104,56,16,"员工号:",16,0);
		Show_Str(10,128,56,16,"电话号:",16,0);
		Show_Str(10,152,56,16,"状  态:",16,0);
		//  **首先寻卡**
		RC522_return = PcdRequest(PICC_REQALL,RC522_ID);
		if( RC522_return != MI_OK)
		{
		}
		
		//  **成功寻到卡**
		if(RC522_return == MI_OK)
		{
			
			//  **防冲撞，防止多卡同时检测**
			if( (PcdAnticoll(RC522_NUM) == MI_OK) )  //确认选择刚才检测到的IC卡
			{
				
				//  **选卡成功**
				if( PcdSelect(RC522_NUM)== MI_OK)
				{
					
					//
					if(PcdAuthState(0x61,0x06,RC522_KEY,RC522_NUM)==MI_OK)
					{
						
						
						LCD_ShowxNum(74,56,RC522_ID[0],3,16,0x80);
						LCD_ShowxNum(106,56,RC522_ID[1],3,16,0x80);
						LCD_ShowxNum(138,56,RC522_ID[2],3,16,0x80);
						LCD_ShowxNum(170,56,RC522_ID[3],3,16,0x80);
												
						LCD_ShowxNum(74,80,RC522_NUM[0],3,16,0x80);
						LCD_ShowxNum(106,80,RC522_NUM[1],3,16,0x80);
						LCD_ShowxNum(138,80,RC522_NUM[2],3,16,0x80);
						LCD_ShowxNum(170,80,RC522_NUM[3],3,16,0x80);

					if(RC522_NUM[0]==179 && RC522_NUM[1]==66 && RC522_NUM[2]==25 && RC522_NUM[3]==174)
						//LCD_ShowxNum(200,180,RC522_NUM[3],3,16,0x80);
					{
					    Show_Str(74,104,60,16,"392001",16,0);
						Show_Str(74,128,100,16,"1710495750",16,0);
						if(stu[0].u_state==0)
		                    Show_Str(74,152,56,16,"未借伞",16,0);
						else
						    Show_Str(74,152,56,16,"已借",16,0);
					}
					}
				}
			}
		}
		LED0=!LED0;
		return KEY_Scan(0);
}


void ui_main(void)
{
	   
 		LCD_Clear(WHITE);
	    POINT_COLOR=BLACK;
        ai_load_picfile("0:/PICTURE/time.bmp",10,32,50,50,1);//显示图片	
	    ai_load_picfile("0:/PICTURE/san.bmp",10,90,50,50,1);//显示图片
	    ai_load_picfile("0:/PICTURE/gong.bmp",10,148,50,50,1);//显示图片	
	    ai_load_picfile("0:/PICTURE/xin.bmp",216,2,20,16,1);//显示图片
	    Show_Str(80,110,96,16,"空余雨伞:",16,0);
	    Show_Str(80,168,96,16,"借伞员工:",16,0);
	    //Show_Str(70,220,48,16,"窗户:",16,0);    
	    Show_Str(2,300,240,16," 员工    刷卡    借伞   还伞",16,0);
		LCD_ShowString(2,2,200,16,16,"    -  -     ");
		LCD_ShowxNum(2,2,calendar.w_year,4,16,0x80);									  
		LCD_ShowxNum(42,2,calendar.w_month,2,16,0x80);									  
		LCD_ShowxNum(66,2,calendar.w_date,2,16,0x80);
	
        //Show_Str(96,40,150,16,"Friday",16,1); 				//显示图片名字
		LCD_ShowString(96,58,200,16,16,"  :  :  ");
		LCD_ShowxNum(96,58,calendar.hour,2,16,0x80);									  
		LCD_ShowxNum(120,58,calendar.min,2,16,0x80);									  
		LCD_ShowxNum(146,58,calendar.sec,2,16,0x80);
		//Show_Str(120,2,120,16,"T:   ℃ H:  %",16,0);
		switch(calendar.week)    //显示 星期机
			{
				case 0:				
				    Show_Str(96,40,200,16," Sunday",16,1); 				
					break;
				case 1:
				    Show_Str(96,40,200,16," Monday",16,1);
					break;
				case 2:					
				    Show_Str(96,40,200,16," Tuesday",16,1);
					break;
				case 3:					
				    Show_Str(96,40,200,16," Wednesday",16,1);
					break;
				case 4:
				    Show_Str(96,40,200,16," Thursday",16,1);
					break;
				case 5:		
				    Show_Str(96,40,200,16," Friday",16,1);
					break;
				case 6:					
				    Show_Str(96,40,200,16," Saturday",16,1);
					break;  
			}
}


void ShaXin_ui(void)  //刷新 时间  
{       
        u8 i=0;
	    u8 c_time=0;
		LCD_ShowString(2,2,200,16,16,"    -  -     ");
		LCD_ShowxNum(2,2,calendar.w_year,4,16,0x80);									  
		LCD_ShowxNum(42,2,calendar.w_month,2,16,0x80);									  
		LCD_ShowxNum(66,2,calendar.w_date,2,16,0x80);

		LCD_ShowString(96,58,200,16,16,"  :  :  ");
		LCD_ShowxNum(96,58,calendar.hour,2,16,0x80);									  
		LCD_ShowxNum(120,58,calendar.min,2,16,0x80);									  
		LCD_ShowxNum(146,58,calendar.sec,2,16,0x80);
	    	    
	    switch(calendar.week)    //显示 星期机
			{
				case 0:				
				    Show_Str(96,40,200,16," Sunday",16,1); 
				
					break;
				case 1:
				    Show_Str(96,40,200,16," Monday",16,1);
					break;
				case 2:					
				    Show_Str(96,40,200,16," Tuesday",16,1);
					break;
				case 3:					
				    Show_Str(96,40,200,16," Wednesday",16,1);
					break;
				case 4:
				    Show_Str(96,40,200,16," Thursday",16,1);
					break;
				case 5:		
				    Show_Str(96,40,200,16," Friday",16,1);
					break;
				case 6:					
				    Show_Str(96,40,200,16," Saturday",16,1);
					break;  
			}
			
		LCD_ShowxNum(156,110,Ur,1,16,0x80);
		LCD_ShowxNum(156,168,4-Ur,1,16,0x80);
		jidian_p(Ur);
		if(Ur<4&&Ur>=0)
		 {
			for(i=0;i<N;i++)
			 {
			  if(stu[i].u_state==1)  //借伞了
			  {
			    c_time=gl_time-stu[i].j_time;
				if(c_time>180)  //大于3分钟 催促还伞
				{
				  stu[i].j_time=gl_time;
				  gsm_borrow_Recall(stu[i].tel_rank,1);  //发送短信还伞
				}
					
			  }				  
			 }	 
		 }
	    delay_ms(200);
}


void  staff_ui(void)
{
	u8 i;
  	LCD_Clear(WHITE);
	LCD_ShowString(2,2,200,16,16,"    -  -     ");
	LCD_ShowxNum(2,2,calendar.w_year,4,16,0x80);									  
	LCD_ShowxNum(42,2,calendar.w_month,2,16,0x80);									  
	LCD_ShowxNum(66,2,calendar.w_date,2,16,0x80);
	ai_load_picfile("0:/PICTURE/xin.bmp",216,2,20,16,1);//显示图片
	
	Show_Str(2,300,240,16,"上一页  下一页   退出   确认",16,0);
	Show_Str(2,26,200,16,"         员工信息表",16,0);
	Show_Str(40,52,48,16,"员工号",16,0);
	Show_Str(120,52,48,16,"手机号",16,0);
	Show_Str(194,52,32,16,"状态",16,0);
	LCD_DrawRectangle(8, 48,230, 290);		 //画矩形
	LCD_DrawLine(8, 72, 230, 72);			//画线
	LCD_DrawLine(8, 96, 230, 96);			//画线
	LCD_DrawLine(8, 120, 230, 120);			//画线
	LCD_DrawLine(8, 144, 230, 144);			//画线
	LCD_DrawLine(8, 168, 230, 168);			//画线
	LCD_DrawLine(8, 192, 230, 192);			//画线
	LCD_DrawLine(8, 216, 230, 216);			//画线
	LCD_DrawLine(8, 240, 230, 240);			//画线
	LCD_DrawLine(8, 264, 230, 264);			//画线
	LCD_DrawLine(8, 288, 230, 288);			//画线
	
	LCD_DrawLine(36, 48, 36, 290);			//画线
	LCD_DrawLine(96, 48, 96, 290);			//画线
	LCD_DrawLine(190, 48, 190, 290);			//画线
    
	if(ye==0)
	{
		for(i=0;i<9;i++)
		{
		  LCD_ShowxNum(12,72+i*24+4,stu[i].tel_rank,2,16,0x80);
		  LCD_ShowxNum(40,72+i*24+4,stu[i].stunum,6,16,0x80);
			
		  LCD_ShowxNum(100,72+i*24+4,stu[i].stutel[0]-'0',1,16,0x80);
		  LCD_ShowxNum(108,72+i*24+4,stu[i].stutel[1]-'0',1,16,0x80);
		  LCD_ShowxNum(116,72+i*24+4,stu[i].stutel[2]-'0',1,16,0x80);
		  LCD_ShowString(124,72+i*24+4,24,16,16,"***");
		  LCD_ShowxNum(148,72+i*24+4,stu[i].stutel[7]-'0',1,16,0x80);
		  LCD_ShowxNum(156,72+i*24+4,stu[i].stutel[8]-'0',1,16,0x80);
		  LCD_ShowxNum(164,72+i*24+4,stu[i].stutel[9]-'0',1,16,0x80);
		  LCD_ShowxNum(172,72+i*24+4,stu[i].stutel[10]-'0',1,16,0x80);
		  if(stu[i].u_state==0) Show_Str(194,72+i*24+4,48,16,"未借",16,0);
		  else if(stu[i].u_state==1) 
		  {
			  POINT_COLOR=RED;
			  Show_Str(194,72+i*24+4,48,16,"已借",16,0);
			  delay_ms(400);
			  POINT_COLOR=BLACK;
		  }			  
		}
	}
	else 
	{
		for(i=9;i<N;i++)
		{
		  LCD_ShowxNum(12,72+(i-9)*24+4,stu[i].tel_rank,2,16,0x80);
		  LCD_ShowxNum(40,72+(i-9)*24+4,stu[i].stunum,6,16,0x80);
			
		  LCD_ShowxNum(100,72+(i-9)*24+4,stu[i].stutel[0]-'0',1,16,0x80);
		  LCD_ShowxNum(108,72+(i-9)*24+4,stu[i].stutel[1]-'0',1,16,0x80);
		  LCD_ShowxNum(116,72+(i-9)*24+4,stu[i].stutel[2]-'0',1,16,0x80);
		  LCD_ShowString(124,72+(i-9)*24+4,24,16,16,"***");
		  LCD_ShowxNum(148,72+(i-9)*24+4,stu[i].stutel[7]-'0',1,16,0x80);
		  LCD_ShowxNum(156,72+(i-9)*24+4,stu[i].stutel[8]-'0',1,16,0x80);
		  LCD_ShowxNum(164,72+(i-9)*24+4,stu[i].stutel[9]-'0',1,16,0x80);
		  LCD_ShowxNum(172,72+(i-9)*24+4,stu[i].stutel[10]-'0',1,16,0x80);
		  if(stu[i].u_state==0) Show_Str(194,72+(i-9)*24+4,48,16,"未借",16,0);
		  else if(stu[i].u_state==1) Show_Str(194,72+(i-9)*24+4,48,16,"已借",16,0);		
		}
	}
}

	
void  umbrella_ui(void)   //  --设置还伞ui
{
	LCD_Clear(WHITE);
	
	Show_Str(2,300,240,16," 一一    退出    重输   确认",16,0);
	Show_Str(8,40,96,16,"请输入员工号",16,0);
	//Show_Str(8,80,96,16,"请输入验证码",16,0);
	LCD_DrawRectangle(110,32,220,64);		 //画矩形
	//LCD_DrawRectangle(110,72,220,104);		 //画矩形
	LCD_DrawRectangle(8, 120,230, 290);		 //画矩形
	LCD_DrawLine(63, 120, 63, 290);			//画线
	LCD_DrawLine(118, 120, 118, 290);			//画线
	LCD_DrawLine(173, 120, 173, 290);			//画线
    LCD_DrawLine(8, 176, 230, 176);			//画线
	LCD_DrawLine(8, 232, 230, 232);			//画线
	
	Show_Str(30,140,240,16,"1      2      3    一一",16,0);
	Show_Str(30,196,240,16,"4      5      6    一一",16,0);
	Show_Str(30,250,240,16,"7      8      9     0",16,0);
	
}

void ui_input(void)   //借伞 ui
{
	LCD_Clear(WHITE);
	Show_Str(2,300,240,16," 一一    退出    重输   确认",16,0);
	Show_Str(8,40,96,16,"请输入员工号",16,0);
	//Show_Str(8,80,96,16,"请输入验证码",16,0);
	LCD_DrawRectangle(110,32,220,64);		 //画矩形
	//LCD_DrawRectangle(110,72,220,104);		 //画矩形
	LCD_DrawRectangle(8, 120,230, 290);		 //画矩形
	LCD_DrawLine(63, 120, 63, 290);			//画线
	LCD_DrawLine(118, 120, 118, 290);			//画线
	LCD_DrawLine(173, 120, 173, 290);			//画线
    LCD_DrawLine(8, 176, 230, 176);			//画线
	LCD_DrawLine(8, 232, 230, 232);			//画线
	
	Show_Str(30,140,240,16,"1      2      3    一一",16,0);
	Show_Str(30,196,240,16,"4      5      6    一一",16,0);
	Show_Str(30,250,240,16,"7      8      9     0",16,0);
	
}

u8 ui_input_s(void)
{
	u8 bri2=0;
	u8 i=0;
	u8 mikey=0;
	numl[0]=0;
	
	for(i=0;i<6;i++)
	{
	     numl[i]=12;
		 while((numl[i]==0)||(numl[i]==12))
		 {
			  bri2=0;
			  numl[i]=ctp_test();
			  //delay_ms(250);
			  mikey=KEY_Scan(0);
			 	 if(mikey==2)
				  {
				   bri2=1;
				   break;
				  }
				  if(mikey==3)  //重输
				  {
				   Show_Str(120+i*16-16,80,16,16," ",16,0);
				   i=i-2;	               		  
				  }
		 }
         if(bri2==1) break; 		 
		 if(numl[i]==11)
		 {		 
			  numl[i]=0;
		 }
		 LCD_ShowxNum(120+i*16,80,numl[i],1,16,0x80);
         delay_ms(250);		 
	}
	
	mikey=0; 
	s_tel=numl[0]*100000+numl[1]*10000+numl[2]*1000+numl[3]*100+numl[4]*10+numl[5];
	if(bri2==1) 
	{
	  mikey=2;
	  s_tel=0;
	}
	while(mikey==0)
	{
	  mikey=KEY_Scan(0);
	}
	 return mikey;
}


u8 ui_input_tel(void)  //输入员工号
{
	u8 i=0;
	u8 bri=0;
	u8 mikey2=0;
	num2[0]=0;
	duan_xin=0;
	for(i=0;i<6;i++)
	{
		 num2[i]=12;
		 while((num2[i]==0) ||(num2[i]==12))
		 {
		  bri=0;
		  num2[i]=ctp_test();
		  mikey2=KEY_Scan(0);
			  if(mikey2==2)
			  {
			   bri=1;
			   break;
			  }
			  if(mikey2==3)
			  {
			   Show_Str(120+i*16-16,40,16,16," ",16,0);
               i=i-2;	               		  
			  }
		  }
		  if(bri==1) break; 
		  if(num2[i]==11)
		  {
		    num2[i]=0;  
		  }
		  if(bri==0)
		  {
			  LCD_ShowxNum(120+i*16,40,num2[i],1,16,0x80);
			  delay_ms(250);		  
		  }
	
	   }
	  
		Show_Str(40,10,200,16,"正在查找...",16,0);
		n_tel=num2[0]*100000+num2[1]*10000+num2[2]*1000+num2[3]*100+num2[4]*10+num2[5];
		//LCD_ShowxNum(10,10,n_tel,6,16,0x80);
		for(i=0;i<10;i++)
		{
		  if(n_tel==stu[i].stunum)
		  {
			  Show_Str(40,10,200,16,"查找成功...     ",16,0);
			  duan_xin=stu[i].tel_rank;  //排名
			  delay_ms(1000);
			  Show_Str(40,10,200,16,"请按确认键...",16,0);
			  break;
		  }		  
		}
		mikey2=0;
		if (duan_xin==0)
		{
			Show_Str(40,10,200,16,"查找错误...     ",16,0);
			delay_ms(1000);
			mikey2=2;
		}
		
		while(mikey2==0)
		{
		  mikey2=KEY_Scan(0);
		}
		 Show_Str(40,10,200,16,"                 ",16,0);
		 return mikey2;
	
}


void panduan_tel(void)
{
	//u8 i=0;
	ui_input();
	delay_ms(400);
    ptel=ui_input_tel();
	if(ptel==2)  //退出
	{
		ui_main();
		pter=0;
	    ptel=0;
	}
	else if(ptel==3)
	{   ptel=0;
		ui_input();
		panduan_tel();
	}
	else if(ptel==4)
	{
			 Show_Str(8,80,96,16,"请输入验证码",16,0);
			 LCD_DrawRectangle(110,72,220,104);		 //画矩形
		     gsm_borrow_Recall(duan_xin,2);                      // 发送借伞短信内容 参数1 tel ,参数2：短信内容
		     //duan_xin=0;	 
		     ui_input_s();
//		     pter=ui_input_s();	     
//			 if(pter==2)  // 退出
//			 {
//				 ui_main();
//				 pter=0;
//				 ptel=0;
//				 
//			 }
//			 else if(pter==3)  //chongshu
//			 {
//				 pter=0;
//				 ui_input();
//				 Show_Str(8,80,96,16,"请输入验证码",16,0);
//				 LCD_DrawRectangle(110,72,220,104);		 //画矩形
//				 for(i=0;i<6;i++)
//				 {LCD_ShowxNum(120+i*16,40,num2[i],1,16,0x80);}
//				 
//				 pter=ui_input_s();
//				 
//				 				 
//			 }
//				else if(pter==4)
//			 {
//				 ui_main();
//				 pter=0;
//				 ptel=0;
//			 }
	}
		
}


void huan_usan(void)
{
  u8 i=0;
  umbrella_ui();
  i=ui_input_tel();
	if(i==2)
    {
	 i=0;
	 ui_main();
	}
	else if(i==4)
	{
	  i=0;
	}
    else if(i==3)
	{
	  huan_usan();
	}
}


u8 ctp_test(void)
{
      u8 t=0;
  	    
            tp_dev.scan(0);

			if(tp_dev.sta&TP_PRES_DOWN)
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{					
					if(tp_dev.x[t]>20&&tp_dev.x[t]<50&&tp_dev.y[t]<160&&tp_dev.y[t]>140)
					{
						return 1;
					}
					else if(tp_dev.x[t]>72&&tp_dev.x[t]<100&&tp_dev.y[t]<160&&tp_dev.y[t]>140)
					{
						return 2;
						
					}
					else if(tp_dev.x[t]>130&&tp_dev.x[t]<156&&tp_dev.y[t]<160&&tp_dev.y[t]>140)
					{
						return 3;
					}
//					else if(tp_dev.x[t]>178&&tp_dev.x[t]<220&&tp_dev.y[t]<160&&tp_dev.y[t]>140)
//					{
//						return 10;
//					}
					//第二行
					else if(tp_dev.x[t]>20&&tp_dev.x[t]<50&&tp_dev.y[t]<216&&tp_dev.y[t]>192)
					{
						return 4;
					}
					else if(tp_dev.x[t]>72&&tp_dev.x[t]<100&&tp_dev.y[t]<216&&tp_dev.y[t]>192)
					{
						return 5;
						
					}
					else if(tp_dev.x[t]>130&&tp_dev.x[t]<156&&tp_dev.y[t]<216&&tp_dev.y[t]>192)
					{
						return 6;
					}
//					else if(tp_dev.x[t]>178&&tp_dev.x[t]<220&&tp_dev.y[t]<220&&tp_dev.y[t]>192)
//					{
//						return 12;
//					}
					//第三行
					else if(tp_dev.x[t]>20&&tp_dev.x[t]<50&&tp_dev.y[t]<280&&tp_dev.y[t]>230)
					{
						return 7;
					}
					else if(tp_dev.x[t]>72&&tp_dev.x[t]<100&&tp_dev.y[t]<280&&tp_dev.y[t]>230)
					{
						return 8;
					}
					else if(tp_dev.x[t]>130&&tp_dev.x[t]<156&&tp_dev.y[t]<280&&tp_dev.y[t]>230)
					{
						return 9;
					}
					else if(tp_dev.x[t]>178&&tp_dev.x[t]<220&&tp_dev.y[t]<280&&tp_dev.y[t]>188)
					{
						return 11;
					}
					else 
						return 0;                    										
				}
				
			} 	
}

