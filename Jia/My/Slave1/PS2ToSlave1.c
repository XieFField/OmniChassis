/**
 * @file		PS2ToSlave1.c
 * @brief		��������PS2���ݸ��ӻ�1��������Դ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��17��
 */


#include <stdio.h>
#include "stm32f10x.h"
#include "Delay/Delay_API_z.h"
#include "PS2_API.h"


// ����Ӳ��USART���DMA���д���ͨѶ
// UART �� USART �������������ڸ�Ӳ���Ƿ��д���ͬ���շ�ģʽ


// ���ݰ���ʽ���壺
// ֡ͷ��0X55
// ID
// ���ݳ���
// ����
// У��λ��֡ͷ+ID+���ݳ���+����

// ������ݰ���ʽ��bug������������ĳ�������ֽ�Ϊ0X55ʱ����ʹ�ӻ�ʶ��Ϊ֡ͷ��ʼ���ݰ��Ľ���
// ����У��λ�Ĵ��ڣ��ӻ�����ɹ����գ����ᵼ�¸����ݰ���ʧ


#define bound		961200


/* ������������������������������������������������������������������������*/
// GPIO�ڳ�ʼ��
#define GPIO_Speed_x		GPIO_Speed_50MHz

#define RCC_TX				RCC_APB2Periph_GPIOA		// TX
#define GPIOx_TX			GPIOA
#define GPIO_Pinx_TX		GPIO_Pin_2

#define RCC_RX				RCC_APB2Periph_GPIOA		// RX
#define GPIOx_RX			GPIOA
#define GPIO_Pinx_RX		GPIO_Pin_3
/* ������������������������������������������������*/


/* ������������������������������������������������������������������������*/
// UART�ڳ�ʼ��
// UART��ѡ������Ϊ1���ر�Ϊ0���������һ����
// UART5û��DMAת�˹��ܣ����������ֹʹ��UART5��
#define USART_1				0			// ������ USARTx �궨������
#define USART_2				1			// ������ USARTx �궨������
#define USART_3				0			// ������ USARTx �궨������
#define UART_4				0			// ������ UARTx �궨������

// UART���շ�ģʽѡ��ѡ�������е�һ����Tx��ֻ��ģʽ��Rx��ֻ��ģʽ��TxAndRx���շ�ģʽ���������һ����
#define	UART_Mode_Tx				0	// ������ USART_Mode_Tx �궨������
#define	UART_Mode_Rx				0	// ������ USART_Mode_Rx �궨������
#define	UART_Mode_TxAndRx			1
/* ������������������������������������������������*/


static uint8_t UART_TXData[26] = { 0x55 , 0x40, 22 };
static volatile uint8_t UART_RXData[26];

static PS2_DataTypeDef PS2_RXData;
static uint8_t* PS2_DataPoint_TXData;


/* ������������������������������������������������������������������������*/
// UART���ж�
#if USART_1
#define RCC_UART 				RCC_APB2Periph_USART1
#define USART_x 				USART1
#elif USART_2
#define RCC_UART 				RCC_APB1Periph_USART2
#define USART_x 				USART2
#elif USART_3
#define RCC_UART 				RCC_APB1Periph_USART3
#define USART_x 				USART3
#elif UART_4
#define RCC_UART 				RCC_APB1Periph_UART4
#define USART_x 				UART4
#endif

// UARTģʽ�ж�
#if			UART_Mode_Tx
#define		UART_Mode_x		USART_Mode_Tx
#elif		UART_Mode_Rx
#define		UART_Mode_x		USART_Mode_Rx
#elif		UART_Mode_TxAndRx
#define		UART_Mode_x		(USART_Mode_Rx | USART_Mode_Tx)
#endif

// DMAͨ���ж�
	// RX
//#if UART_Mode_Rx || UART_Mode_TxAndRx
#if USART_1
#define RX_DMAx_Channely	DMA1_Channel5
#elif USART_2
#define RX_DMAx_Channely	DMA1_Channel6
#elif USART_3
#define RX_DMAx_Channely	DMA1_Channel3
#elif UART_4
#define RX_DMAx_Channely	DMA2_Channel3
#endif
//#endif
	// TX
#if UART_Mode_Tx || UART_Mode_TxAndRx
#if USART_1
#define TX_DMAx_Channely	DMA1_Channel4
#elif USART_2
#define TX_DMAx_Channely	DMA1_Channel7
#elif USART_3
#define TX_DMAx_Channely	DMA1_Channel2
#elif UART_4
#define TX_DMAx_Channely	DMA2_Channel5
#endif
#endif
/* ������������������������������������������������*/


void PS2ToSlave1_Init(void)
{
	// GPIO������
		// ����GPIOʱ��
#if UART_Mode_Tx || UART_Mode_TxAndRx
	RCC_APB2PeriphClockCmd(RCC_TX, ENABLE);
#endif
#if UART_Mode_Rx || UART_Mode_TxAndRx
	RCC_APB2PeriphClockCmd(RCC_RX, ENABLE);
#endif

	// GPIO��ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;

#if UART_Mode_Tx || UART_Mode_TxAndRx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_TX;				// TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// ���ø����������ģʽ
	GPIO_Init(GPIOx_TX, &GPIO_InitStructure);
#endif

#if UART_Mode_Rx || UART_Mode_TxAndRx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_RX;				// RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	// ���ø�������ģʽ
	GPIO_Init(GPIOx_RX, &GPIO_InitStructure);
#endif

	// USART������
		// ����USARTʱ��
#if USART_1
	RCC_APB2PeriphClockCmd(RCC_UART, ENABLE);
#elif USART_2 || USART_3 || UART_4
	RCC_APB1PeriphClockCmd(RCC_UART, ENABLE);
#endif

	// USART��ʼ��
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;										// ���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// ��Ӳ������������
	USART_InitStructure.USART_Mode = UART_Mode_x;									// �ա������շ�ģʽѡ��
	USART_Init(USART_x, &USART_InitStructure);

	// ʹ��
	USART_Cmd(USART_x, ENABLE);								// ʹ�ܴ���

	// DMA����
		// ����DMAʱ��
#if UART_4
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);		// ����DMA2��ʱ��
#elif USART_1 || USART_2 || USART_3
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		// ����DMA1��ʱ��
#endif

	// DMA��ʼ��
	DMA_InitTypeDef DMA_InitStructure;											// ����ṹ�����
		// TX
#if UART_Mode_Tx || UART_Mode_TxAndRx
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART_x->DR;			// �������ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		// �������ݿ�ȣ�ѡ���ֽ�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			// �����ַ������ѡ��ʧ�ܣ�ʼ����USART��DR�Ĵ���ΪĿ�ĵ�
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART_TXData;				// �洢������ַ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				// �洢�����ݿ�ȣ�ѡ���ֽڣ����������ݿ�ȶ�Ӧ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						// �洢����ַ������ѡ��ʹ�ܣ�ÿ��ת�˺������Ƶ���һ��λ��
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;							// ���ݴ��䷽��ѡ���ɴ洢�������裬����ת���������ݼĴ���
	DMA_InitStructure.DMA_BufferSize = 0;										// ת�˵����ݴ�С��ת�˴�����
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								// ģʽ��ѡ�񵥴�ģʽ
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								// �洢�����洢����ѡ��ʧ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						// ���ȼ���ѡ���е�
	DMA_Init(TX_DMAx_Channely, &DMA_InitStructure);
#endif

		// RX
#if UART_Mode_Rx || UART_Mode_TxAndRx
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART_x->DR;			// �������ַ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		// �������ݿ�ȣ�ѡ���ֽ�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			// �����ַ������ѡ��ʧ�ܣ�ʼ����USART��DR�Ĵ���ΪĿ�ĵ�
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART_RXData;				// �洢������ַ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				// �洢�����ݿ�ȣ�ѡ���ֽڣ����������ݿ�ȶ�Ӧ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						// �洢����ַ������ѡ��ʹ�ܣ�ÿ��ת�˺������Ƶ���һ��λ��
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							// ���ݴ��䷽��ѡ�������赽�洢�����������ݼĴ���ת������
	DMA_InitStructure.DMA_BufferSize = 26;										// ת�˵����ݴ�С��ת�˴�����
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								// ģʽ��ѡ��ѭ��ģʽ
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								// �洢�����洢����ѡ��ʧ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							// ���ȼ���ѡ���
	DMA_Init(RX_DMAx_Channely, &DMA_InitStructure);
#endif

	// DMAʹ��
		// TX
#if UART_Mode_Tx || UART_Mode_TxAndRx
	USART_DMACmd(USART_x, USART_DMAReq_Tx, ENABLE);			// ʹ��USART��TX��DMAת��
#endif

		// RX
#if UART_Mode_Rx || UART_Mode_TxAndRx
	DMA_Cmd(RX_DMAx_Channely, ENABLE);						// DMAʹ��
	USART_DMACmd(USART_x, USART_DMAReq_Rx, ENABLE);			// ʹ��USART��RX��DMAת��
#endif
}

void PS2ToSlave1_SetPoint_PS2Data(uint8_t* DataPoint)
{
	PS2_DataPoint_TXData = DataPoint;
}

void PS2ToSlave1_SendData(void)
{
	uint8_t Sum;

	Sum = 0;

	for (uint8_t i = 0; i < 22; i++)
	{
		UART_TXData[i + 3] = *(PS2_DataPoint_TXData + i);
	}

	for (uint8_t i = 0; i < 25; i++)
	{
		Sum += UART_TXData[i];
	}

	UART_TXData[25] = Sum;

	DMA_Cmd(TX_DMAx_Channely, DISABLE);					// ʧ��DMA��ֹͣ��������

	DMA_SetCurrDataCounter(TX_DMAx_Channely, 26);		// ���ô��䳤��

	DMA_Cmd(TX_DMAx_Channely, ENABLE);					// ʹ��DMA����ʼ��������
}

PS2_DataTypeDef* PS2FromMaster_GetPoint_PS2Data(void)
{
	return &PS2_RXData;
}

/**
  * @brief  ��ȡ����������PS2����
  * @param  ��
  * @retval (int8_t)�Ƿ�ɹ���ȡPS2���ݣ��ɹ�����1��ʧ�ܷ���-1
  * @note	�����ȡ�ɹ������µ����ݸ�����һ������
  * @note	�����ȡʧ�ܣ��򱣳���һ������
  * @note	��ȡDMAת�˴�������ʱ��DMA�ᱻʧ�ܣ�ֹͣ�����µ����ݣ�ֱ����ȡ�������ʹ��
  * @note	�ڴ��ڼ䣬��������µ����ݣ������޷����ն���ʧ
  * @note	����֮��DMAֻ��һ�ػ��壬Ҫ���Ӧ��ʹ��˫�ػ��壬�����Ҳ������
  */
int8_t PS2FromMaster_ReadData(void)
{
	uint8_t DataPacket_DataLength = 0;
	uint8_t DataPacket_Data[22];
	uint8_t DataPacket_Sum = 0;

	// ���������˺�ATK_IMU901ģ���������������Ƶ�����
	// ������ڳ����н���DMAʧ�ܣ��ڽ��մ�������ʱ
	// ��������˶���ʱ���нϴ�ĸ��ʻᵼ��DMAת��ʧЧ����Ҫ�����ϵ縴λ
	//DMA_Cmd(RX_DMAx_Channely, DISABLE);			// ʧ��DMA��ֹͣ��������

	uint8_t SuccessfullyReadFlagBit = 0;

	for (uint8_t i = 0; i < 26; i++)
	{
		if (UART_RXData[i] == 0x55)		// �ж�֡ͷ0x55
		{
			DataPacket_Sum += UART_RXData[i];

			i++;

			if (i == 26)	// �ж��Ƿ񵽴�����ĩβ
			{
				i = 0;
			}

			if (UART_RXData[i] == 0x40)		// �ж�ID
			{
				DataPacket_Sum += UART_RXData[i];

				i++;

				if (i == 26)	// �ж��Ƿ񵽴�����ĩβ
				{
					i = 0;
				}

				DataPacket_DataLength = UART_RXData[i];

				for (uint8_t j = 0; j < DataPacket_DataLength; j++)
				{
					DataPacket_Sum += UART_RXData[i];

					i++;

					if (i == 26)	// �ж��Ƿ񵽴�����ĩβ
					{
						i = 0;
					}

					DataPacket_Data[j] = UART_RXData[i];

				}

				DataPacket_Sum += UART_RXData[i];

				i++;

				if (i == 26)	// �ж��Ƿ񵽴�����ĩβ
				{
					i = 0;
				}

				if (DataPacket_Sum != UART_RXData[i])
				{
					DMA_Cmd(RX_DMAx_Channely, ENABLE);			// ʹ��DMA����ʼ��������
					return -1;			// ��ȡʧ�ܣ����ش���ֵ-1
				}
				else
				{
					SuccessfullyReadFlagBit++;
					break;
				}
			}
		}
	}

	//DMA_Cmd(RX_DMAx_Channely, ENABLE);			// ʹ��DMA����ʼ��������

	if (!SuccessfullyReadFlagBit)
	{
		return -1;			// ��ȡʧ�ܣ����ش���ֵ-1
	}

	PS2_RXData.PS2_ID			=		DataPacket_Data[0];
	PS2_RXData.PS2_Mode			=		DataPacket_Data[1];
	PS2_RXData.PS2_SELECT		=		DataPacket_Data[2];
	PS2_RXData.PS2_L3			=		DataPacket_Data[3];
	PS2_RXData.PS2_R3			=		DataPacket_Data[4];
	PS2_RXData.PS2_START		=		DataPacket_Data[5];
	PS2_RXData.PS2_UP			=		DataPacket_Data[6];
	PS2_RXData.PS2_RIGHT		=		DataPacket_Data[7];
	PS2_RXData.PS2_DOWN			=		DataPacket_Data[8];
	PS2_RXData.PS2_LEFT			=		DataPacket_Data[9];
	PS2_RXData.PS2_L2			=		DataPacket_Data[10];
	PS2_RXData.PS2_R2			=		DataPacket_Data[11];
	PS2_RXData.PS2_L1			=		DataPacket_Data[12];
	PS2_RXData.PS2_R1			=		DataPacket_Data[13];
	PS2_RXData.PS2_Triangle		=		DataPacket_Data[14];
	PS2_RXData.PS2_Circle		=		DataPacket_Data[15];
	PS2_RXData.PS2_Fork			=		DataPacket_Data[16];
	PS2_RXData.PS2_Rectangle	=		DataPacket_Data[17];
	PS2_RXData.PS2_RX			=		DataPacket_Data[18];
	PS2_RXData.PS2_RY			=		DataPacket_Data[19];
	PS2_RXData.PS2_LX			=		DataPacket_Data[20];
	PS2_RXData.PS2_LY			=		DataPacket_Data[21];

	return 1;		// ��ȡ�ɹ�����������ֵ1
}

