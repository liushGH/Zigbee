#ifndef DATA_H_
#define DATA_H_


/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "AT24C256.h"
#include "Congfig.h"


/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */

/******************״̬��ʶ*********************/
#define DATA_OK                   1     //�����ɹ�
#define DATA_EXIST                2     //�����Ѿ�����
#define DATA_FULL                 3     //�洢��������
#define DATA_NULL                 4     //�洢�����ѿ�
#define DATA_ERR                  0     //����ʧ��



/* ------------------------------------------------------------------------------------------------
 *                                          Application
 * ------------------------------------------------------------------------------------------------
 */

void Data_BaseID_Init(void);
void Data_BaseID_Read(uint8_t *BaseId);



/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
//uint8_t Data_Storage(uint8_t *WriteId, uint16_t WriteAddr);


#endif