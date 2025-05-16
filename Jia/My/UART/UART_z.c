/**
 * @file		UART_z.c
 * @brief		����(�첽�жϵ�Ե�)��������Դ�ļ�
 * @platform	keil5ƽ̨STM32F103RCT6�ͺŵ�Ƭ��
 * @mainpage	������ѧԺʮ�ڻ�����������R1��������Ƭ��(�ͺ�F103RCT6)
 * @author		ɣ��
 * @date		2025��4��10��
 */


#ifndef __UART_z_h
#define __UART_z_h


// ����Ӳ��USART���д���ͨѶ
// ʹ���жϽ�������ת��
// UART �� USART �������������ڸ�Ӳ���Ƿ��д���ͬ���շ�ģʽ


#include <stdio.h>
#include "stm32f10x.h"
#include "UART_Def_z.h"


/* ������������������������������������������������������������������������*/
// GPIO�ڳ�ʼ��
#define GPIO_Speed_x		GPIO_Speed_50MHz

#define RCC_TX				RCC_APB2Periph_GPIOC		// TX
#define GPIOx_TX			GPIOC
#define GPIO_Pinx_TX		GPIO_Pin_12

#define RCC_RX				RCC_APB2Periph_GPIOC		// RX
#define GPIOx_RX			GPIOC
#define GPIO_Pinx_RX		GPIO_Pin_11
/* ������������������������������������������������*/


/* ������������������������������������������������������������������������*/
// UART�ڳ�ʼ��
// UART��ѡ������Ϊ1���ر�Ϊ0���������һ����
#define USART_1				0			// ������ USARTx �궨������
#define USART_2				0			// ������ USARTx �궨������
#define USART_3				0			// ������ USARTx �궨������
#define UART_4				0			// ������ UARTx �궨������
#define UART_5				1			// ������ UARTx �궨������

// UART���շ�ģʽѡ��ѡ�������е�һ����Tx��ֻ��ģʽ��Rx��ֻ��ģʽ��TxAndRx���շ�ģʽ���������һ����
#define	UART_Mode_Tx				1	// ������ USART_Mode_Tx �궨������
#define	UART_Mode_Rx				0	// ������ USART_Mode_Rx �궨������
#define	UART_Mode_TxAndRx			0
/* ������������������������������������������������*/


/* ������������������������������������������������������������������������*/
static volatile uint8_t UART_RX_ReadFlagBit = 0;			// �������ݴ���ȡ��־λ

static volatile uint8_t FlagBit_0x0d = 0;					// 0x0d��־λ����
static volatile uint8_t FlagBit_0x0a = 0;					// 0x0a��־λ����

static volatile UART_DataTypeDef UART_OriginalRXData;
static UART_DataTypeDef UART_ExternalRXData;
/* ������������������������������������������������*/


/* ������������������������������������������������������������������������*/
// GPIO�ڵ�ƽ����
#if UART_Mode_Tx || UART_Mode_TxAndRx
#define TX_H		(GPIO_WriteBit(GPIOx_TX, GPIO_Pinx_TX, (BitAction)1))		// TX
#define TX_L		(GPIO_WriteBit(GPIOx_TX, GPIO_Pinx_TX, (BitAction)0))
#endif

/* ������������������������������������������������*/


/* ������������������������������������������������������������������������*/
// UART���ж�
#if USART_1
#define IRQn_USARTx 			USART1_IRQn
#define RCC_UART 				RCC_APB2Periph_USART1
#define USART_x 				USART1
#define USARTx_IRQHandler 		USART1_IRQHandler
#elif USART_2
#define IRQn_USARTx 			USART2_IRQn
#define RCC_UART 				RCC_APB1Periph_USART2
#define USART_x 				USART2
#define USARTx_IRQHandler 		USART2_IRQHandler
#elif USART_3
#define IRQn_USARTx 			USART3_IRQn
#define RCC_UART 				RCC_APB1Periph_USART3
#define USART_x 				USART3
#define USARTx_IRQHandler 		USART3_IRQHandler
#elif UART_4
#define IRQn_USARTx 			UART4_IRQn
#define RCC_UART 				RCC_APB1Periph_UART4
#define USART_x 				UART4
#define USARTx_IRQHandler 		UART4_IRQHandler
#elif UART_5
#define IRQn_USARTx 			UART5_IRQn
#define RCC_UART 				RCC_APB1Periph_UART5
#define USART_x 				UART5
#define USARTx_IRQHandler 		UART5_IRQHandler
#endif

// UARTģʽ�ж�
#if			UART_Mode_Tx
#define		UART_Mode_x		USART_Mode_Tx
#elif		UART_Mode_Rx
#define		UART_Mode_x		USART_Mode_Rx
#elif		UART_Mode_TxAndRx
#define		UART_Mode_x		(USART_Mode_Rx | USART_Mode_Tx)
#endif
/* ������������������������������������������������*/


/**
  * @brief  ���ڳ�ʼ��
  * @param  bound ������
  * @retval ��
  */
void MyUART_Init(uint32_t bound)
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

	// ����GPIO��ʼ״̬
#if UART_Mode_Tx || UART_Mode_TxAndRx
	TX_H;
#endif

	// �����ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// NVIC����
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = IRQn_USARTx;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		// ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				// ��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART������
		// ����USARTʱ��
#if USART_1
	RCC_APB2PeriphClockCmd(RCC_UART, ENABLE);
#elif USART_2 || USART_3 || UART_4 || UART_5
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
	USART_ITConfig(USART_x, USART_IT_RXNE, ENABLE);		// ʹ�ܴ��ڽ����ж�
	USART_Cmd(USART_x, ENABLE);							// ʹ�ܴ���
}

void USARTx_IRQHandler(void)
{
	if (USART_GetITStatus(USART_x, USART_IT_RXNE) == SET)		// �ж��ж�
	{
		uint8_t Res;													// ��Ҳ��֪�� Res ��ʲô�ļ�д
		Res = USART_ReceiveData(USART_x);						// ��ȡ���յ�������

		// �жϽ��յ������ݣ�������0x0d 0x0a��β
		if (FlagBit_0x0a == 0)			// ���0x0a��־λ
		{
			if (FlagBit_0x0d)						// ��һ�ν��յ���0x0d
			{
				if (Res != 0x0a)								// ��һ��û���յ�0x0a
				{
					UART_OriginalRXData.UART_RX_Count = 0;									// ���մ����������״̬��Ǻͼ��������¿�ʼ
					FlagBit_0x0d = 0;
					FlagBit_0x0a = 0;
				}
				else											// ��һ�ν��յ���0x0a
					FlagBit_0x0a = 1;												// ��0x0a��־λ������������
			}
			else									// ��һ�λ�û�յ�0X0d
			{
				if (Res == 0x0d)								// ��һ�ν��յ���0x0d
				{
					FlagBit_0x0d = 1;												// ��0x0d��־λ
					UART_RX_ReadFlagBit = 1;										// �ô������ݴ���ȡ��־λ
				}
				else											// ��һ��û���յ�0x0d
				{
					UART_OriginalRXData.UART_RX_BUF[UART_OriginalRXData.UART_RX_Count] = Res;		// ������һ�ε�����
					UART_OriginalRXData.UART_RX_Count++;									// ���������յ�����Ч�ֽ���Ŀ������ֵ
					if (UART_OriginalRXData.UART_RX_Count > (UART_REC_LEN - 1))						// �жϼ���ֵ�Ƿ����
						UART_OriginalRXData.UART_RX_Count = 0;														// ���մ����������״̬��ǣ����¿�ʼ
				}
			}
		}
	}

	USART_ClearFlag(USART_x, USART_IT_RXNE);					// ����жϱ�־λ
}

// printf()����ض��򵽴���
// ��Ҳ��֪ʲôԭ�������ü���
// ʹ��ACSII������ʽ����
int fputc(int ch, FILE* f)
{
	USART_SendData(USART_x, (uint8_t)ch);

	while (USART_GetFlagStatus(USART_x, USART_FLAG_TC) == RESET) {}

	return ch;
}

volatile uint8_t* UART_GetPoint_RX_ReadFlagBit(void)
{
	return &UART_RX_ReadFlagBit;
}

UART_DataTypeDef* UART_GetPoint_RXData(void)
{
	return &UART_ExternalRXData;
}

/**
  * @brief  ��ȡ���ڽ��յ�����
  * @param  ��
  * @retval ��
  */
void UART_ReadData(void)
{
	UART_ExternalRXData = UART_OriginalRXData;

	FlagBit_0x0d = 0;						// ���0x0d��־λ����
	FlagBit_0x0a = 0;						// ���0x0a��־λ����
	UART_RX_ReadFlagBit = 0;				// ��մ������ݴ���ȡ��־λ
	UART_OriginalRXData.UART_RX_Count = 0;	// ��մ��ڽ����ֽڼ���
}


#endif
