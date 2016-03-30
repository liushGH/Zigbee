#ifndef SPI_H_
#define SPI_H_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include<ioCC2530.h>

/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */
/*SPI口引脚设定*/
#define CS      P1_2  //片选信号   
#define SCK     P1_3  //时钟信号
#define MOSI    P0_2  //CC2530输出，MFRC522输入
#define MISO    P0_3  //CC2530输入，MFRC522输出





/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
void Spi_Delay(uint8_t n);
void Spi_IO_Init(void);
void Spi_Init(void);
uint8_t Spi_ReadByte(void);
void Spi_WriteByte(uint8_t Spi_Tx);


#endif
