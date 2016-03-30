#define RF_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Rf.h"



/**************************************************************************************************
 * @fn          Rf_Init
 * @brief       射频模块初始化
 * @param       无
 * @return      无
 **************************************************************************************************
 */
void Rf_Init(void)
{
  //FRMCTRL0  |=  (0x20|0x40);   //硬件CRC校验和AUTO_ACK(自动回复确认帧)使能
  //使能AUTO_ACK标志位使得CC2530可以自动应答无线数据帧，这种应答机制需要遵循802.15.4标准，
  //若使能自动应答，转发送的数据包也必须符合802.15.4标准,所以禁止该位
  FRMCTRL0  |=    0x40;          //硬件检测一个CRC-16 
  FRMFILT0   =    0x0C;          //静止接收过滤，即接收所有数据包 
  //TXPOWER    =    0xD5;          //发射功率1db
  TXPOWER    =    0xF5;          //发射功率最大4.5db
  FREQCTRL   =    0x0B;          //信道选择，选择11信道，f=2405Mhz 
  
  CCACTRL0   =    0xF8;          //推荐值smartRF软件生成
  FSCAL1     =    0x00;          //获得最佳EVM 
  TXFILTCFG  =    0x09;          //设置TX抗混叠过滤器以获得合适的带宽 WEB  
  AGCCTRL1   =    0x15;          //调整AGC的目标值
  AGCCTRL2   =    0xFE; 

  RFIRQM0   |=   (1<<6);         //接收一个完整的帧中断使能  
  //IEN2      |=   (1<<0);         //RF一般中断使能 WEB
  //EA         =    1;
 
  RFST       =    0xED;          //清除RXFIFO缓冲区并复位解调器
  RFST       =    0xE3;          //RF接收使能
  
//  RFST       =    0xEF;            //RF接收关闭
  
  //Fram   = FRMCTRL0;           //rest以后的值是0x40，默认硬件CRC校验
}




/**************************************************************************************************
 * @fn          Rf_Enable
 * @brief       射频模块接收使能,需要注意的是发送使能已经在Send函数中
 * @param       无
 * @return      无
 **************************************************************************************************
 */
void Rf_RxEnable(void)
{
  RFST       =    0xED;          //清除RXFIFO缓冲区并复位解调器
  RFST       =    0xE3;          //RF接收使能
}



/**************************************************************************************************
 * @fn          Rf_RxDisable
 * @brief       射频接收发送禁止，需要注意的是发送使能已经在Send函数中
 * @param       无
 * @return      无
 **************************************************************************************************
 */
void Rf_Disable(void)
{
  RFST       =    0xEF;          //禁用RX/TX和频率合成器
}





/**************************************************************************************************
 * @fn          Rf_Send
 * @brief       射频模块数据发送
 * @param       无
 * @return      无
 **************************************************************************************************
 */
/*------------------------------------------------------------------------------------------------
 *          物理同步头          |                   帧负载                 |  帧尾（校验序列）
 *------------------------------------------------------------------------------------------------
 *  帧引导序列  | 帧开始界定符  | 帧长度（LEN）| MAC数据帧头 |  MAC帧负载  |  16bit寄存器（2字节）
 *------------------------------------------------------------------------------------------------
 *          硬件产生            |                本函数产生（RFD）         |     软硬件CRC校验
 *----------------------------------------------------------------------------------------------*/

void Rf_Send(uint8_t *pbuf, uint8_t TxLength)
{
  uint16_t i;
  RFST        =    0xE3;               //RF接收使能
  while( FSMSTAT1&((1<<1)|(1<<5)));    //开启发送状态
  RFIRQM0    &=  ~(1<<6);              //禁止接收数据包中断  
  IEN2       &=  ~(1<<0);              //清除RF全局中断  
  RFST        =    0xEE;               //清除TXFIFO发送缓冲区 ISFLUSHTX  
  RFIRQF1     =  ~(1<<1);              //清除发送完成标志  
 
  RFD         =    TxLength + 2;       //填充缓冲区填充过程需要增加2字节，CRC校验自动填充 
 
  for(i=0; i<TxLength; i++)  
  {  
    RFD=*pbuf++;                       //数据写入RFD寄存器就是写入TXFIFO 读取RFD就是读取RXFIFO
                                       //RXFIFO TXFIFO 最大128byte
  }
  RFST =0xE9;                          //发送数据包 ISTXON 使能TX
  while(!(RFIRQF1&(1<<1)));            //等待发送完成 
  RFIRQF1     =  ~(1<<1);              //清除发送完成标志
  RFIRQM0    |=   (1<<6);              //打开RX接收中断
  IEN2       |=   (1<<0);              //RF中断使能 
  //Rf_Delay(20000);                     //指示LED,临时的
}


/**************************************************************************************************
 * @fn          Rf_Receive
 * @brief       射频模块数据接收
 * @param       无
 * @return      无
 **************************************************************************************************
 */

/*------------------------------------------------------------------------------------------------
 *          物理同步头          |                   帧负载                 |  帧尾（校验序列）
 *------------------------------------------------------------------------------------------------
 *  帧引导序列  | 帧开始界定符  | 帧长度（LEN）| MAC数据帧头 |  MAC帧负载  |  16bit寄存器（2字节）
 *------------------------------------------------------------------------------------------------
 *          硬件移除            |                本函数接收（RFD）         |     RSSI和CRC_FLAG
 *----------------------------------------------------------------------------------------------*/
void Rf_Receive(uint8_t *RF_Rxbuff)
{
  uint8_t i;
  uint8_t RxLength  = 0;
  uint8_t CrcFlag   = 0;
  int16_t Rssi;
  
  RxLength  = RFD - 2;                //去除最后两个校验位RSSI和CRC
  RxLength &= 0x7F;                   //RXFIFO的最大字节数是128 
  
  for(i=0; i<RxLength; i++)
  {
    RF_Rxbuff[i]    = RFD;
    Rf_Delay(200);                    //需要注意这个新加的延时
  }
  
  Rssi      = RFD - 73;               //读取RSSI结果，接收信号强度,单位是db
  CrcFlag   = RFD;                    //CRC校验结果只占用BIT7位
  
  Rssi += 100;                        //先变成正数！！！！
  
  
  if(CrcFlag & 0x80)                  //CRC校验正确
  {
    //Uart0_SendString((uint8_t *)RF_Rxbuff,RxLength);
    //Uart0_SendByte(Rssi);
  }
  
  else                                //CRC校验失败
  {
    //Uart0_SendByte(0x11);
    //Uart0_SendByte(0x22);
    //Uart0_SendByte(0x55);
  }
}


/**************************************************************************************************
 * @fn          Rf_Delay
 * @brief       delay time function
 * @param       无
 * @return      无
 **************************************************************************************************
 */

void Rf_Delay(uint16_t t)
{
  uint16_t tt;
  for(tt=0; tt<t; tt++);
  for(tt=0; tt<t; tt++);
  for(tt=0; tt<t; tt++);
  for(tt=0; tt<t; tt++);
  for(tt=0; tt<t; tt++);
}
