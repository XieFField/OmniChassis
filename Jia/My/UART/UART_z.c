/**
 * @file		UART_z.c
 * @brief		串口(异步中断点对点)操作函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月10日
 */


#ifndef __UART_z_h
#define __UART_z_h


// 采用硬件USART进行串口通讯
// 使用中断进行数据转运
// UART 和 USART 的意义区别在于该硬件是否有串口同步收发模式


#include <stdio.h>
#include "stm32f10x.h"
#include "UART_Def_z.h"


/* ————————————————————————————————————*/
// GPIO口初始化
#define GPIO_Speed_x		GPIO_Speed_50MHz

#define RCC_TX				RCC_APB2Periph_GPIOC		// TX
#define GPIOx_TX			GPIOC
#define GPIO_Pinx_TX		GPIO_Pin_12

#define RCC_RX				RCC_APB2Periph_GPIOC		// RX
#define GPIOx_RX			GPIOC
#define GPIO_Pinx_RX		GPIO_Pin_11
/* ————————————————————————*/


/* ————————————————————————————————————*/
// UART口初始化
// UART口选择，启用为1，关闭为0，最多启用一个！
#define USART_1				0			// 避免与 USARTx 宏定义重名
#define USART_2				0			// 避免与 USARTx 宏定义重名
#define USART_3				0			// 避免与 USARTx 宏定义重名
#define UART_4				0			// 避免与 UARTx 宏定义重名
#define UART_5				1			// 避免与 UARTx 宏定义重名

// UART口收发模式选择，选择三个中的一个，Tx是只发模式，Rx是只收模式，TxAndRx是收发模式，最多启用一个！
#define	UART_Mode_Tx				1	// 避免与 USART_Mode_Tx 宏定义重名
#define	UART_Mode_Rx				0	// 避免与 USART_Mode_Rx 宏定义重名
#define	UART_Mode_TxAndRx			0
/* ————————————————————————*/


/* ————————————————————————————————————*/
static volatile uint8_t UART_RX_ReadFlagBit = 0;			// 串口数据待读取标志位

static volatile uint8_t FlagBit_0x0d = 0;					// 0x0d标志位变量
static volatile uint8_t FlagBit_0x0a = 0;					// 0x0a标志位变量

static volatile UART_DataTypeDef UART_OriginalRXData;
static UART_DataTypeDef UART_ExternalRXData;
/* ————————————————————————*/


/* ————————————————————————————————————*/
// GPIO口电平设置
#if UART_Mode_Tx || UART_Mode_TxAndRx
#define TX_H		(GPIO_WriteBit(GPIOx_TX, GPIO_Pinx_TX, (BitAction)1))		// TX
#define TX_L		(GPIO_WriteBit(GPIOx_TX, GPIO_Pinx_TX, (BitAction)0))
#endif

/* ————————————————————————*/


/* ————————————————————————————————————*/
// UART口判断
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

// UART模式判断
#if			UART_Mode_Tx
#define		UART_Mode_x		USART_Mode_Tx
#elif		UART_Mode_Rx
#define		UART_Mode_x		USART_Mode_Rx
#elif		UART_Mode_TxAndRx
#define		UART_Mode_x		(USART_Mode_Rx | USART_Mode_Tx)
#endif
/* ————————————————————————*/


/**
  * @brief  串口初始化
  * @param  bound 波特率
  * @retval 无
  */
void MyUART_Init(uint32_t bound)
{
	// GPIO口设置
		// 开启GPIO时钟
#if UART_Mode_Tx || UART_Mode_TxAndRx
	RCC_APB2PeriphClockCmd(RCC_TX, ENABLE);
#endif
#if UART_Mode_Rx || UART_Mode_TxAndRx
	RCC_APB2PeriphClockCmd(RCC_RX, ENABLE);
#endif

	// GPIO初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;

#if UART_Mode_Tx || UART_Mode_TxAndRx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_TX;				// TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 配置复用推挽输出模式
	GPIO_Init(GPIOx_TX, &GPIO_InitStructure);
#endif

#if UART_Mode_Rx || UART_Mode_TxAndRx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pinx_RX;				// RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	// 配置浮空输入模式
	GPIO_Init(GPIOx_RX, &GPIO_InitStructure);
#endif

	// 设置GPIO初始状态
#if UART_Mode_Tx || UART_Mode_TxAndRx
	TX_H;
#endif

	// 设置中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// NVIC设置
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = IRQn_USARTx;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				// 响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART口设置
		// 开启USART时钟
#if USART_1
	RCC_APB2PeriphClockCmd(RCC_UART, ENABLE);
#elif USART_2 || USART_3 || UART_4 || UART_5
	RCC_APB1PeriphClockCmd(RCC_UART, ENABLE);
#endif

	// USART初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;										// 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 无硬件数据流控制
	USART_InitStructure.USART_Mode = UART_Mode_x;									// 收、发或收发模式选择
	USART_Init(USART_x, &USART_InitStructure);

	// 使能
	USART_ITConfig(USART_x, USART_IT_RXNE, ENABLE);		// 使能串口接受中断
	USART_Cmd(USART_x, ENABLE);							// 使能串口
}

void USARTx_IRQHandler(void)
{
	if (USART_GetITStatus(USART_x, USART_IT_RXNE) == SET)		// 判断中断
	{
		uint8_t Res;													// 我也不知道 Res 是什么的简写
		Res = USART_ReceiveData(USART_x);						// 读取接收到的数据

		// 判断接收到的数据，必须是0x0d 0x0a结尾
		if (FlagBit_0x0a == 0)			// 检查0x0a标志位
		{
			if (FlagBit_0x0d)						// 上一次接收到了0x0d
			{
				if (Res != 0x0a)								// 这一次没接收到0x0a
				{
					UART_OriginalRXData.UART_RX_Count = 0;									// 接收错误，清除接受状态标记和计数，重新开始
					FlagBit_0x0d = 0;
					FlagBit_0x0a = 0;
				}
				else											// 这一次接收到了0x0a
					FlagBit_0x0a = 1;												// 置0x0a标志位，代表接收完毕
			}
			else									// 上一次还没收到0X0d
			{
				if (Res == 0x0d)								// 这一次接收到了0x0d
				{
					FlagBit_0x0d = 1;												// 置0x0d标志位
					UART_RX_ReadFlagBit = 1;										// 置串口数据待读取标志位
				}
				else											// 这一次没接收到0x0d
				{
					UART_OriginalRXData.UART_RX_BUF[UART_OriginalRXData.UART_RX_Count] = Res;		// 储存这一次的数据
					UART_OriginalRXData.UART_RX_Count++;									// 自增“接收到的有效字节数目”计数值
					if (UART_OriginalRXData.UART_RX_Count > (UART_REC_LEN - 1))						// 判断计数值是否溢出
						UART_OriginalRXData.UART_RX_Count = 0;														// 接收错误，清除接收状态标记，重新开始
				}
			}
		}
	}

	USART_ClearFlag(USART_x, USART_IT_RXNE);					// 清空中断标志位
}

// printf()输出重定向到串口
// 我也不知什么原理，拿来用即可
// 使用ACSII编码形式发送
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
  * @brief  读取串口接收的数据
  * @param  无
  * @retval 无
  */
void UART_ReadData(void)
{
	UART_ExternalRXData = UART_OriginalRXData;

	FlagBit_0x0d = 0;						// 清空0x0d标志位变量
	FlagBit_0x0a = 0;						// 清空0x0a标志位变量
	UART_RX_ReadFlagBit = 0;				// 清空串口数据待读取标志位
	UART_OriginalRXData.UART_RX_Count = 0;	// 清空串口接收字节计数
}


#endif
