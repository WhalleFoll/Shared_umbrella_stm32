 
#include "rc522.h"
#include "delay.h"
 
 
 
//  **使用正点原子的延时 ，大概率程序卡死在 222行do-while循环 寻卡与认证等待时间**
#define   RC522_DELAY()  Delay_ns(200)
// ************************************************************************************************************** //
void Delay_ns(u32 ns)  //可以自行Debug调试时间
{
	while(ns--)
	{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}
}
// ************************************************************************************************************** //
//向RC522发送一字节数据
void RC522_SPI_SendData(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if (data & 0x80)
      RC522_MOSI = 1;
    else 
      RC522_MOSI = 0;
		
    RC522_DELAY();
    RC522_SCK = 0;
    RC522_DELAY();
    RC522_SCK = 1;
    RC522_DELAY();
    data <<= 1; 
	}
}
// ************************************************************************************************************** //
//读取RC522的值
u8 RC522_SPI_ReadData()
{
	u8 t;
  u8 Data;
 
  for(t=0;t<8;t++)
  {
    Data <<= 1;
    RC522_SCK = 0;
    RC522_DELAY();
    if(RC522_MISO ==1) // // //
        Data |= 0x01;
		
    RC522_DELAY();
    RC522_SCK = 1;
    RC522_DELAY();
  }
  return Data;
}
// ************************************************************************************************************** //
//读RC522寄存器的值
//addr 寄存器地址
u8 RC522_Read_Reg(u8 regaddr)
{
	u8 ucaddr;  //寄存器有效地址
	u8 return_value; //寄存器返回值
	
	ucaddr = ((regaddr<<1) & 0x7E) | 0x80;	 //  **变成有效地址形式，最低位为0，最高位为1 从RC522中读出数据
	RC522_CS = 0;  //0为使能
	RC522_SPI_SendData(ucaddr);  //发送有效地址
	return_value = RC522_SPI_ReadData();  //收到寄存去返回值
	RC522_CS = 1;  //1为失能
	
	return return_value;
}
// ************************************************************************************************************** //
//写RC522寄存器的值
//regaddr  寄存器地址
//regvalue  寄存器中的值
void RC522_Write_Reg(u8 regaddr,u8 regvalue)
{
	u8 ucaddr;  //寄存器有效地址
	
	ucaddr = (regaddr<<1) & 0x7E;	   //  **变成有效地址形式，最低位为0，最高位为0 从EC522中写入数据
	RC522_CS = 0;
	RC522_SPI_SendData(ucaddr);   //发送地址
	RC522_SPI_SendData(regvalue);  //发送数据
	RC522_CS = 1;	
}
// ************************************************************************************************************** //
//对RC522寄存器置位，相当于对MF522中的地址写数据，控制不同的操作
//reg_add 寄存器地址
//w_val  置位值
void RC522_SetBit(u8 reg_add, u8 reg_val)  
{
  u8 ucTemp;
	
  ucTemp=RC522_Read_Reg(reg_add);  //读取当前地址寄存器的值
  RC522_Write_Reg(reg_add,ucTemp | reg_val);         // set bit mask
}
// ************************************************************************************************************** //
//对RC522寄存器清位，清除MF522寄存器中的值  还原之前数据，或者换为无用值
//reg_add 寄存器地址
//w_val  清位值
void RC522_ClearBit(u8 reg_add,u8 reg_val)
{
  u8 ucTemp;
	
  ucTemp=RC522_Read_Reg(reg_add);   //读取当前地址寄存器的值
  RC522_Write_Reg(reg_add,ucTemp & (~reg_val));  // clear bit mask
}
// ************************************************************************************************************** //
//开启天线 
void PcdAntenna_On()
{
  u8 uc;
  
  uc=RC522_Read_Reg(TxControlReg);  //读取 TxControlReg 的值
  if (!(uc & 0x03))                 //盘丢按是否为正确值
	RC522_SetBit(TxControlReg,0x03); //否则写入正确值
}
// ************************************************************************************************************** //
//关闭天线
void PcdAntenna_Off()
{
  RC522_ClearBit(TxControlReg,0x03);	 //清除写入值
}
// ************************************************************************************************************** //
//复位RC522 
void Pcd_Reset()
{
	RC522_RST = 1;
	//delay_us(1);
	Delay_ns(10);
	RC522_RST = 0;
	//delay_us(1);
	Delay_ns(10);
	RC522_RST = 1;
	//delay_us(1);
	Delay_ns(10);
	
	RC522_Write_Reg(CommandReg,0x0f);
	RC522_Write_Reg(CommandReg,0x0f);   //  **写入两遍 ，防止出错**
	//while(RC522_Read_Reg(CommandReg) & 0x10); //  **使用等待 ，可能出错 ，初始化不通过**
	//delay_us(1);
	Delay_ns(10);
	
  RC522_Write_Reg(ModeReg,0x3D);           //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
  RC522_Write_Reg(TReloadRegL,30);         //16位定时器低位
	RC522_Write_Reg(TReloadRegH,0);			     //16位定时器高位
  RC522_Write_Reg(TModeReg,0x8D);				   //定义内部定时器的设置
  RC522_Write_Reg(TPrescalerReg,0x3E);		 //设置定时器分频系数
	RC522_Write_Reg(TxAutoReg,0x40);				 //调制发送信号为100%ASK
}
// ************************************************************************************************************** //
//设置RC522的工作方式
//Type 工作方式选择
void M500PcdConfigISOType(u8 Type)
{
	if (Type=='A')                     //ISO14443_A
  {
		RC522_ClearBit(Status2Reg,0x08);  //状态2寄存器
		
    RC522_Write_Reg(ModeReg,0x3D);    //3F  和Mifare卡通讯，CRC初始值0x6363
		RC522_Write_Reg(RxSelReg,0x86);   //84  选择内部接收器设置，内部模拟部分调制信号，发送数据后延迟6个位时钟接收
		RC522_Write_Reg(RFCfgReg,0x7F);   //4F  配置接收器 48dB最大增益
		RC522_Write_Reg(TReloadRegL,30);  //tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		RC522_Write_Reg(TReloadRegH,0);
		RC522_Write_Reg(TModeReg,0x8D);
		RC522_Write_Reg(TPrescalerReg,0x3E);
		//delay_us(2);
		Delay_ns(1000);
		
		PcdAntenna_On();//开天线
  }	 
}
// ************************************************************************************************************** //
//通过RC522和ISO14443卡通讯
//ucCommand  RC522命令字
//pInData  通过RC522发送到卡片的数据
//ucInLenByte  发送数据的字节长度
//pOutData  接收到的卡片返回数据
//pOutLenBit  返回数据的位长度
//状态值= MI_OK  成功
char PcdComMF522(u8 ucCommand,u8 *pInData,u8 ucInLenByte,u8 *pOutData,u32 *pOutLenBit)
{
  char cStatus = MI_ERR;
  u8 ucIrqEn   = 0x00;
  u8 ucWaitFor = 0x00;
  u8 ucLastBits;
  u8 ucN;
  u32 ul;
	
  switch(ucCommand)
  {
     case PCD_AUTHENT:		  //Mifare认证
        ucIrqEn   = 0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
        ucWaitFor = 0x10;		//认证寻卡等待时候 查询空闲中断标志位
        break;
     
     case PCD_TRANSCEIVE:		//接收发送 发送接收
        ucIrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
        break;
     
     default:
       break;     
  }
  RC522_Write_Reg(ComIEnReg,ucIrqEn | 0x80);		//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反 
  RC522_ClearBit(ComIrqReg,0x80);			//Set1该位清零时，CommIRqReg的屏蔽位清零
  RC522_Write_Reg(CommandReg,PCD_IDLE);		//写空闲命令
  RC522_SetBit(FIFOLevelReg,0x80);			//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
 
  for(ul=0;ul<ucInLenByte;ul++)
  {  RC522_Write_Reg(FIFODataReg,pInData[ul]); }   		//写数据进FIFOdata
    
  RC522_Write_Reg(CommandReg,ucCommand);					//写命令
 
 
  if(ucCommand==PCD_TRANSCEIVE)
	{  RC522_SetBit(BitFramingReg,0x80); } 				//StartSend置位启动数据发送 该位与收发命令使用时才有效
 
  ul = 800;//根据时钟频率调整，操作M1卡最大等待时间25ms
	
  do 														//认证 与寻卡等待时间	
  {
    ucN = RC522_Read_Reg(ComIrqReg);							//查询事件中断
    ul--;
  }
	while((ul != 0)&&(!(ucN & 0x01))&&(!(ucN & ucWaitFor)));		//退出条件i=0,定时器中断，与写空闲命令
 
  RC522_ClearBit(BitFramingReg,0x80);					//清理允许StartSend位
 
  if(ul != 0)
  {
    if(!(RC522_Read_Reg(ErrorReg) & 0x1B))			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
    {
      cStatus = MI_OK;
      
      if(ucN & ucIrqEn & 0x01)					//是否发生定时器中断
        cStatus = MI_NOTAGERR;   
        
      if(ucCommand==PCD_TRANSCEIVE)
      {
        ucN=RC522_Read_Reg(FIFOLevelReg);			//读FIFO中保存的字节数
        
        ucLastBits = RC522_Read_Reg(ControlReg) & 0x07;	//最后接收到得字节的有效位数
        
        if(ucLastBits)
          *pOutLenBit = (ucN-1) * 8 + ucLastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
        else
          *pOutLenBit = ucN*8;   					//最后接收到的字节整个字节有效
        
        if(ucN==0)		
          ucN = 1;    
        
        if(ucN > MAXRLEN)
          ucN = MAXRLEN;   
        
        for(ul=0;ul<ucN;ul++)
          pOutData[ul]=RC522_Read_Reg(FIFODataReg);
        }
    }
    else
      cStatus = MI_ERR;       
  }
 
  RC522_SetBit(ControlReg,0x80);    // stop timer now
  RC522_Write_Reg(CommandReg,PCD_IDLE); 
   
  return cStatus;
}
// ************************************************************************************************************** //
//  **寻卡**
//ucReq_code，寻卡方式 = 0x52，寻感应区内所有符合14443A标准的卡；寻卡方式= 0x26，寻未进入休眠状态的卡
//pTagType，卡片类型代码
//  = 0x4400，Mifare_UltraLight
//  = 0x0400，Mifare_One(S50)
//  = 0x0200，Mifare_One(S70)
//  = 0x0800，Mifare_Pro(X))
//  = 0x4403，Mifare_DESFire
//状态值= MI_OK，成功
char PcdRequest(u8 ucReq_code,u8 *pTagType)
{
  char cStatus;  
  u8 ucComMF522Buf[MAXRLEN]; 
	u32 ulLen;
 
 
  RC522_ClearBit(Status2Reg,0x08);	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
  RC522_Write_Reg(BitFramingReg,0x07);	//	发送的最后一个字节的 七位
  RC522_SetBit(TxControlReg,0x03);	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
 
  ucComMF522Buf[0] = ucReq_code;		//存入 卡片命令字
 
  cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&ulLen);	//寻卡
 
  if((cStatus==MI_OK)&&(ulLen==0x10))	//寻卡成功返回卡类型 
  {
    *pTagType       = ucComMF522Buf[0];
    *(pTagType + 1) = ucComMF522Buf[1];
  }
  else
   cStatus = MI_ERR;
 
  return cStatus;
}
// ************************************************************************************************************** //
//防冲撞
//pSnr，卡片序列号，4字节
//状态值= MI_OK，成功
char PcdAnticoll(u8 *pSnr)
{
  char cStatus;
  u8 uc,ucSnr_check=0;
  u8 ucComMF522Buf[MAXRLEN]; 
  u32 ulLen;
  
 
  RC522_ClearBit(Status2Reg,0x08);		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
  RC522_Write_Reg(BitFramingReg,0x00);		//清理寄存器 停止收发
  RC522_ClearBit(CollReg,0x80);			//清ValuesAfterColl所有接收的位在冲突后被清除
 
  ucComMF522Buf[0] = 0x93;	//卡片防冲突命令
  ucComMF522Buf[1] = 0x20;
 
  cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&ulLen);//与卡片通信
 
  if(cStatus == MI_OK)		//通信成功
  {
    for(uc=0;uc<4;uc++)
    {
      *(pSnr + uc) = ucComMF522Buf[uc];			//读出UID
      ucSnr_check ^= ucComMF522Buf[uc];
    }
    
    if(ucSnr_check != ucComMF522Buf[uc])
    cStatus = MI_ERR;    				 
  }
  RC522_SetBit(CollReg,0x80);
      
  return cStatus;		
}
 
// ************************************************************************************************************** //
//用RC522计算CRC16
//pIndata，计算CRC16的数组
//ucLen，计算CRC16的数组字节长度
//pOutData，存放计算结果存放的首地址
void CalulateCRC(u8 *pIndata,u8 ucLen,u8 *pOutData)
{
  u8 uc,ucN;
 
  RC522_ClearBit(DivIrqReg,0x04);
  RC522_Write_Reg(CommandReg,PCD_IDLE);
  RC522_SetBit(FIFOLevelReg,0x80);
 
  for(uc=0;uc<ucLen;uc++)
    RC522_Write_Reg(FIFODataReg,*(pIndata + uc));   
 
  RC522_Write_Reg(CommandReg,PCD_CALCCRC);
  uc = 0xFF;
 
  do 
  {
    ucN = RC522_Read_Reg(DivIrqReg);
    uc--;
  } 
	while((uc != 0) && !(ucN & 0x04));
  
  pOutData[0] = RC522_Read_Reg(CRCResultRegL);
  pOutData[1] = RC522_Read_Reg(CRCResultRegM);
}
 
// ************************************************************************************************************** //
//选定卡片
//pSnr，卡片序列号，4字节
//状态值= MI_OK，成功
char PcdSelect(u8 *pSnr)
{
  char ucN;
  u8 uc;
  u8 ucComMF522Buf [ MAXRLEN ]; 
  u32  ulLen;
  
  
  ucComMF522Buf[0] = PICC_ANTICOLL1;
  ucComMF522Buf[1] = 0x70;
  ucComMF522Buf[6] = 0;
 
  for(uc=0;uc<4;uc++)
  {
    ucComMF522Buf[uc + 2] = *(pSnr + uc);
    ucComMF522Buf[6] ^= *(pSnr + uc);
  }
  
  CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
 
  RC522_ClearBit(Status2Reg,0x08);
 
  ucN = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&ulLen);
  
  if((ucN==MI_OK)&&(ulLen==0x18))
    ucN = MI_OK;  
  else
    ucN = MI_ERR;    
  
  return ucN;		
}
// ************************************************************************************************************** //
//验证卡片密码
//ucAuth_mode，密码验证模式= 0x60，验证A密钥，密码验证模式= 0x61，验证B密钥
//uint8_t ucAddr，块地址
//pKey，密码
//pSnr，卡片序列号，4字节状态值= MI_OK，成功
char PcdAuthState(u8 ucAuth_mode,u8 ucAddr,u8 *pKey,u8 *pSnr)
{
  char cStatus;
  u8 uc, ucComMF522Buf[MAXRLEN];
  u32 ulLen;
  
  ucComMF522Buf[0] = ucAuth_mode;
  ucComMF522Buf[1] = ucAddr;
 
  for(uc=0;uc<6;uc++)
    ucComMF522Buf[uc + 2] = *(pKey + uc);   
 
  for(uc=0;uc<6;uc++)
    ucComMF522Buf[uc + 8] = *(pSnr + uc);  
	
   //memcpy(&ucComMF522Buf[2], pKey, 6); 
   //memcpy(&ucComMF522Buf[8], pSnr, 4); 	
 
  cStatus = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&ulLen);
 
  if((cStatus != MI_OK)||(!(RC522_Read_Reg(Status2Reg) & 0x08)))
    cStatus = MI_ERR;   
    
  return cStatus;
}
 
// ************************************************************************************************************** //
//写数据到M1卡一块
//uint8_t ucAddr，块地址
//pData，写入的数据，16字节
//状态值= MI_OK，成功
char PcdWrite(u8 ucAddr,u8 *pData)
{
  char cStatus;
  u8 uc, ucComMF522Buf[MAXRLEN];
  u32 ulLen;
   
  
  ucComMF522Buf[0] = PICC_WRITE;
  ucComMF522Buf[1] = ucAddr;
 
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
  cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&ulLen);
 
  if((cStatus != MI_OK)||(ulLen != 4)||((ucComMF522Buf[0] & 0x0F) != 0x0A))
	  cStatus = MI_ERR;
      
  if(cStatus==MI_OK)
  {
    //memcpy(ucComMF522Buf, pData, 16);
    for(uc=0;uc<16;uc++)
      ucComMF522Buf[uc] = *(pData + uc);  
    
    CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
 
    cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&ulLen);
    
    if((cStatus != MI_OK)||(ulLen != 4)||((ucComMF522Buf[0] & 0x0F) != 0x0A))
      cStatus = MI_ERR;   
  }
	
  return cStatus;		
}
 
// ************************************************************************************************************** //
//读取M1卡一块数据
//ucAddr，块地址
//pData，读出的数据，16字节
//状态值= MI_OK，成功
char PcdRead(u8 ucAddr,u8 *pData)
{
  char cStatus;
  u8 uc, ucComMF522Buf[MAXRLEN]; 
  u32 ulLen;
  
  ucComMF522Buf[0] = PICC_READ;
  ucComMF522Buf[1] = ucAddr;
 
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
  cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&ulLen);
 
  if((cStatus==MI_OK)&&(ulLen==0x90))
  {
    for(uc=0;uc<16;uc++)
      *(pData + uc) = ucComMF522Buf[uc];
  }
  else
    cStatus = MI_ERR;   
   
  return cStatus;		
}
 
// ************************************************************************************************************** //
//命令卡片进入休眠状态
//状态值= MI_OK，成功
char PcdHalt()
{
	u8 ucComMF522Buf[MAXRLEN]; 
	u32  ulLen;
  
  ucComMF522Buf[0] = PICC_HALT;
  ucComMF522Buf[1] = 0;
	
  CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 	PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&ulLen);
 
  return MI_OK;	
}
// ************************************************************************************************************** //
//RC522初始化
void RC522_Init()
{
	SPI_init();	
	RC522_RST = 1;  //失能
	RC522_CS = 1;   //失能
}
// ************************************************************************************************************** //
void SPI_init(void)
{
  GPIO_InitTypeDef GPIO_Initspi;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );
	
	//复位，即即RC522模块的RST引脚，接STM32的普通IO即可
	GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_4;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  GPIO_Init(GPIOC,&GPIO_Initspi);
	
	//片选，即RC522模块的SDA引脚
	//  **正点原子对应引脚可以原理图查看**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_4;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  GPIO_Init(GPIOA,&GPIO_Initspi);
	
	//时钟，即RC522模块的SCK引脚,接STM32的SPI的SCK引脚
	//  **正点原子对应引脚可以原理图查看**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_5;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  GPIO_Init(GPIOA,&GPIO_Initspi);
	
	// 数据输入，即即RC522模块的MOSI引脚，接STM32的SPI的MOSI引脚
	//  **正点原子对应引脚可以原理图查看**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_7;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  GPIO_Init(GPIOA,&GPIO_Initspi);	
	
	// 数据输出，即即RC522模块的MISO引脚，接STM32的SPI的MISO引脚
	//  **正点原子对应引脚可以原理图查看**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_6;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //浮空输入
  GPIO_Init(GPIOA,&GPIO_Initspi);
}

