 
#include "rc522.h"
#include "delay.h"
 
 
 
//  **ʹ������ԭ�ӵ���ʱ ������ʳ������� 222��do-whileѭ�� Ѱ������֤�ȴ�ʱ��**
#define   RC522_DELAY()  Delay_ns(200)
// ************************************************************************************************************** //
void Delay_ns(u32 ns)  //��������Debug����ʱ��
{
	while(ns--)
	{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}
}
// ************************************************************************************************************** //
//��RC522����һ�ֽ�����
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
//��ȡRC522��ֵ
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
//��RC522�Ĵ�����ֵ
//addr �Ĵ�����ַ
u8 RC522_Read_Reg(u8 regaddr)
{
	u8 ucaddr;  //�Ĵ�����Ч��ַ
	u8 return_value; //�Ĵ�������ֵ
	
	ucaddr = ((regaddr<<1) & 0x7E) | 0x80;	 //  **�����Ч��ַ��ʽ�����λΪ0�����λΪ1 ��RC522�ж�������
	RC522_CS = 0;  //0Ϊʹ��
	RC522_SPI_SendData(ucaddr);  //������Ч��ַ
	return_value = RC522_SPI_ReadData();  //�յ��Ĵ�ȥ����ֵ
	RC522_CS = 1;  //1Ϊʧ��
	
	return return_value;
}
// ************************************************************************************************************** //
//дRC522�Ĵ�����ֵ
//regaddr  �Ĵ�����ַ
//regvalue  �Ĵ����е�ֵ
void RC522_Write_Reg(u8 regaddr,u8 regvalue)
{
	u8 ucaddr;  //�Ĵ�����Ч��ַ
	
	ucaddr = (regaddr<<1) & 0x7E;	   //  **�����Ч��ַ��ʽ�����λΪ0�����λΪ0 ��EC522��д������
	RC522_CS = 0;
	RC522_SPI_SendData(ucaddr);   //���͵�ַ
	RC522_SPI_SendData(regvalue);  //��������
	RC522_CS = 1;	
}
// ************************************************************************************************************** //
//��RC522�Ĵ�����λ���൱�ڶ�MF522�еĵ�ַд���ݣ����Ʋ�ͬ�Ĳ���
//reg_add �Ĵ�����ַ
//w_val  ��λֵ
void RC522_SetBit(u8 reg_add, u8 reg_val)  
{
  u8 ucTemp;
	
  ucTemp=RC522_Read_Reg(reg_add);  //��ȡ��ǰ��ַ�Ĵ�����ֵ
  RC522_Write_Reg(reg_add,ucTemp | reg_val);         // set bit mask
}
// ************************************************************************************************************** //
//��RC522�Ĵ�����λ�����MF522�Ĵ����е�ֵ  ��ԭ֮ǰ���ݣ����߻�Ϊ����ֵ
//reg_add �Ĵ�����ַ
//w_val  ��λֵ
void RC522_ClearBit(u8 reg_add,u8 reg_val)
{
  u8 ucTemp;
	
  ucTemp=RC522_Read_Reg(reg_add);   //��ȡ��ǰ��ַ�Ĵ�����ֵ
  RC522_Write_Reg(reg_add,ucTemp & (~reg_val));  // clear bit mask
}
// ************************************************************************************************************** //
//�������� 
void PcdAntenna_On()
{
  u8 uc;
  
  uc=RC522_Read_Reg(TxControlReg);  //��ȡ TxControlReg ��ֵ
  if (!(uc & 0x03))                 //�̶����Ƿ�Ϊ��ȷֵ
	RC522_SetBit(TxControlReg,0x03); //����д����ȷֵ
}
// ************************************************************************************************************** //
//�ر�����
void PcdAntenna_Off()
{
  RC522_ClearBit(TxControlReg,0x03);	 //���д��ֵ
}
// ************************************************************************************************************** //
//��λRC522 
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
	RC522_Write_Reg(CommandReg,0x0f);   //  **д������ ����ֹ����**
	//while(RC522_Read_Reg(CommandReg) & 0x10); //  **ʹ�õȴ� �����ܳ��� ����ʼ����ͨ��**
	//delay_us(1);
	Delay_ns(10);
	
  RC522_Write_Reg(ModeReg,0x3D);           //���巢�ͺͽ��ճ���ģʽ ��Mifare��ͨѶ��CRC��ʼֵ0x6363
  RC522_Write_Reg(TReloadRegL,30);         //16λ��ʱ����λ
	RC522_Write_Reg(TReloadRegH,0);			     //16λ��ʱ����λ
  RC522_Write_Reg(TModeReg,0x8D);				   //�����ڲ���ʱ��������
  RC522_Write_Reg(TPrescalerReg,0x3E);		 //���ö�ʱ����Ƶϵ��
	RC522_Write_Reg(TxAutoReg,0x40);				 //���Ʒ����ź�Ϊ100%ASK
}
// ************************************************************************************************************** //
//����RC522�Ĺ�����ʽ
//Type ������ʽѡ��
void M500PcdConfigISOType(u8 Type)
{
	if (Type=='A')                     //ISO14443_A
  {
		RC522_ClearBit(Status2Reg,0x08);  //״̬2�Ĵ���
		
    RC522_Write_Reg(ModeReg,0x3D);    //3F  ��Mifare��ͨѶ��CRC��ʼֵ0x6363
		RC522_Write_Reg(RxSelReg,0x86);   //84  ѡ���ڲ����������ã��ڲ�ģ�ⲿ�ֵ����źţ��������ݺ��ӳ�6��λʱ�ӽ���
		RC522_Write_Reg(RFCfgReg,0x7F);   //4F  ���ý����� 48dB�������
		RC522_Write_Reg(TReloadRegL,30);  //tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		RC522_Write_Reg(TReloadRegH,0);
		RC522_Write_Reg(TModeReg,0x8D);
		RC522_Write_Reg(TPrescalerReg,0x3E);
		//delay_us(2);
		Delay_ns(1000);
		
		PcdAntenna_On();//������
  }	 
}
// ************************************************************************************************************** //
//ͨ��RC522��ISO14443��ͨѶ
//ucCommand  RC522������
//pInData  ͨ��RC522���͵���Ƭ������
//ucInLenByte  �������ݵ��ֽڳ���
//pOutData  ���յ��Ŀ�Ƭ��������
//pOutLenBit  �������ݵ�λ����
//״ֵ̬= MI_OK  �ɹ�
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
     case PCD_AUTHENT:		  //Mifare��֤
        ucIrqEn   = 0x12;		//��������ж�����ErrIEn  ��������ж�IdleIEn
        ucWaitFor = 0x10;		//��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
        break;
     
     case PCD_TRANSCEIVE:		//���շ��� ���ͽ���
        ucIrqEn   = 0x77;		//����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
        break;
     
     default:
       break;     
  }
  RC522_Write_Reg(ComIEnReg,ucIrqEn | 0x80);		//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴ 
  RC522_ClearBit(ComIrqReg,0x80);			//Set1��λ����ʱ��CommIRqReg������λ����
  RC522_Write_Reg(CommandReg,PCD_IDLE);		//д��������
  RC522_SetBit(FIFOLevelReg,0x80);			//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����
 
  for(ul=0;ul<ucInLenByte;ul++)
  {  RC522_Write_Reg(FIFODataReg,pInData[ul]); }   		//д���ݽ�FIFOdata
    
  RC522_Write_Reg(CommandReg,ucCommand);					//д����
 
 
  if(ucCommand==PCD_TRANSCEIVE)
	{  RC522_SetBit(BitFramingReg,0x80); } 				//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч
 
  ul = 800;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
	
  do 														//��֤ ��Ѱ���ȴ�ʱ��	
  {
    ucN = RC522_Read_Reg(ComIrqReg);							//��ѯ�¼��ж�
    ul--;
  }
	while((ul != 0)&&(!(ucN & 0x01))&&(!(ucN & ucWaitFor)));		//�˳�����i=0,��ʱ���жϣ���д��������
 
  RC522_ClearBit(BitFramingReg,0x80);					//��������StartSendλ
 
  if(ul != 0)
  {
    if(!(RC522_Read_Reg(ErrorReg) & 0x1B))			//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
    {
      cStatus = MI_OK;
      
      if(ucN & ucIrqEn & 0x01)					//�Ƿ�����ʱ���ж�
        cStatus = MI_NOTAGERR;   
        
      if(ucCommand==PCD_TRANSCEIVE)
      {
        ucN=RC522_Read_Reg(FIFOLevelReg);			//��FIFO�б�����ֽ���
        
        ucLastBits = RC522_Read_Reg(ControlReg) & 0x07;	//�����յ����ֽڵ���Чλ��
        
        if(ucLastBits)
          *pOutLenBit = (ucN-1) * 8 + ucLastBits;   	//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
        else
          *pOutLenBit = ucN*8;   					//�����յ����ֽ������ֽ���Ч
        
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
//  **Ѱ��**
//ucReq_code��Ѱ����ʽ = 0x52��Ѱ��Ӧ�������з���14443A��׼�Ŀ���Ѱ����ʽ= 0x26��Ѱδ��������״̬�Ŀ�
//pTagType����Ƭ���ʹ���
//  = 0x4400��Mifare_UltraLight
//  = 0x0400��Mifare_One(S50)
//  = 0x0200��Mifare_One(S70)
//  = 0x0800��Mifare_Pro(X))
//  = 0x4403��Mifare_DESFire
//״ֵ̬= MI_OK���ɹ�
char PcdRequest(u8 ucReq_code,u8 *pTagType)
{
  char cStatus;  
  u8 ucComMF522Buf[MAXRLEN]; 
	u32 ulLen;
 
 
  RC522_ClearBit(Status2Reg,0x08);	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
  RC522_Write_Reg(BitFramingReg,0x07);	//	���͵����һ���ֽڵ� ��λ
  RC522_SetBit(TxControlReg,0x03);	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�
 
  ucComMF522Buf[0] = ucReq_code;		//���� ��Ƭ������
 
  cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&ulLen);	//Ѱ��
 
  if((cStatus==MI_OK)&&(ulLen==0x10))	//Ѱ���ɹ����ؿ����� 
  {
    *pTagType       = ucComMF522Buf[0];
    *(pTagType + 1) = ucComMF522Buf[1];
  }
  else
   cStatus = MI_ERR;
 
  return cStatus;
}
// ************************************************************************************************************** //
//����ײ
//pSnr����Ƭ���кţ�4�ֽ�
//״ֵ̬= MI_OK���ɹ�
char PcdAnticoll(u8 *pSnr)
{
  char cStatus;
  u8 uc,ucSnr_check=0;
  u8 ucComMF522Buf[MAXRLEN]; 
  u32 ulLen;
  
 
  RC522_ClearBit(Status2Reg,0x08);		//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
  RC522_Write_Reg(BitFramingReg,0x00);		//����Ĵ��� ֹͣ�շ�
  RC522_ClearBit(CollReg,0x80);			//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����
 
  ucComMF522Buf[0] = 0x93;	//��Ƭ����ͻ����
  ucComMF522Buf[1] = 0x20;
 
  cStatus = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&ulLen);//�뿨Ƭͨ��
 
  if(cStatus == MI_OK)		//ͨ�ųɹ�
  {
    for(uc=0;uc<4;uc++)
    {
      *(pSnr + uc) = ucComMF522Buf[uc];			//����UID
      ucSnr_check ^= ucComMF522Buf[uc];
    }
    
    if(ucSnr_check != ucComMF522Buf[uc])
    cStatus = MI_ERR;    				 
  }
  RC522_SetBit(CollReg,0x80);
      
  return cStatus;		
}
 
// ************************************************************************************************************** //
//��RC522����CRC16
//pIndata������CRC16������
//ucLen������CRC16�������ֽڳ���
//pOutData����ż�������ŵ��׵�ַ
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
//ѡ����Ƭ
//pSnr����Ƭ���кţ�4�ֽ�
//״ֵ̬= MI_OK���ɹ�
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
//��֤��Ƭ����
//ucAuth_mode��������֤ģʽ= 0x60����֤A��Կ��������֤ģʽ= 0x61����֤B��Կ
//uint8_t ucAddr�����ַ
//pKey������
//pSnr����Ƭ���кţ�4�ֽ�״ֵ̬= MI_OK���ɹ�
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
//д���ݵ�M1��һ��
//uint8_t ucAddr�����ַ
//pData��д������ݣ�16�ֽ�
//״ֵ̬= MI_OK���ɹ�
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
//��ȡM1��һ������
//ucAddr�����ַ
//pData�����������ݣ�16�ֽ�
//״ֵ̬= MI_OK���ɹ�
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
//���Ƭ��������״̬
//״ֵ̬= MI_OK���ɹ�
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
//RC522��ʼ��
void RC522_Init()
{
	SPI_init();	
	RC522_RST = 1;  //ʧ��
	RC522_CS = 1;   //ʧ��
}
// ************************************************************************************************************** //
void SPI_init(void)
{
  GPIO_InitTypeDef GPIO_Initspi;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );
	
	//��λ������RC522ģ���RST���ţ���STM32����ͨIO����
	GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_4;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
  GPIO_Init(GPIOC,&GPIO_Initspi);
	
	//Ƭѡ����RC522ģ���SDA����
	//  **����ԭ�Ӷ�Ӧ���ſ���ԭ��ͼ�鿴**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_4;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
  GPIO_Init(GPIOA,&GPIO_Initspi);
	
	//ʱ�ӣ���RC522ģ���SCK����,��STM32��SPI��SCK����
	//  **����ԭ�Ӷ�Ӧ���ſ���ԭ��ͼ�鿴**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_5;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
  GPIO_Init(GPIOA,&GPIO_Initspi);
	
	// �������룬����RC522ģ���MOSI���ţ���STM32��SPI��MOSI����
	//  **����ԭ�Ӷ�Ӧ���ſ���ԭ��ͼ�鿴**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_7;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
  GPIO_Init(GPIOA,&GPIO_Initspi);	
	
	// �������������RC522ģ���MISO���ţ���STM32��SPI��MISO����
	//  **����ԭ�Ӷ�Ӧ���ſ���ԭ��ͼ�鿴**
  GPIO_Initspi.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Initspi.GPIO_Pin = GPIO_Pin_6;
  GPIO_Initspi.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //��������
  GPIO_Init(GPIOA,&GPIO_Initspi);
}

