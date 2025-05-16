/**
 * @file		PS2ToSlave1.c
 * @brief		主机发送PS2数据给从机1操作函数源文件
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月17日
 */


#include <stdio.h>
#include "stm32f10x.h"
#include "Delay/Delay_API_z.h"
#include "PS2_API.h"


// 采用硬件USART配合DMA进行串口通讯
// UART 和 USART 的意义区别在于该硬件是否有串口同步收发模式


// 数据包格式定义：
// 帧头：0X55
// ID
// 数据长度
// 数据
// 校验位：帧头+ID+数据长度+数据

// 这个数据包格式有bug，容易在其中某个数据字节为0X55时，会使从机识别为帧头开始数据包的接收
// 由于校验位的存在，从机不会成功接收，但会导致该数据包丢失


#define bound		961200


/* ————————————————————————————————————*/
// GPIO口初始化
#define GPIO_Speed_x		GPIO_Speed_50MHz

#define RCC_TX				RCC_APB2Periph_GPIOA		// TX
#define GPIOx_TX			GPIOA
#define GPIO_Pinx_TX		GPIO_Pin_2

#define RCC_RX				RCC_APB2Periph_GPIOA		// RX
#define GPIOx_RX			GPIOA
#define GPIO_Pinx_RX		GPIO_Pin_3
/* ————————————————————————*/


/* ————————————————————————————————————*/
// UART口初始化
// UART口选择，启用为1，关闭为0，最多启用一个！
// UART5没有DMA转运功能！所以这里禁止使用UART5！
#define USART_1				0			// 避免与 USARTx 宏定义重名
#define USART_2				1			// 避免与 USARTx 宏定义重名
#define USART_3				0			// 避免与 USARTx 宏定义重名
#define UART_4				0			// 避免与 UARTx 宏定义重名

// UART口收发模式选择，选择三个中的一个，Tx是只发模式，Rx是只收模式，TxAndRx是收发模式，最多启用一个！
#define	UART_Mode_Tx				0	// 避免与 USART_Mode_Tx 宏定义重名
#define	UART_Mode_Rx				0	// 避免与 USART_Mode_Rx 宏定义重名
#define	UART_Mode_TxAndRx			1
/* ————————————————————————*/


static uint8_t UART_TXData[26] = { 0x55 , 0x40, 22 };
static volatile uint8_t UART_RXData[26];

static PS2_DataTypeDef PS2_RXData;
static uint8_t* PS2_DataPoint_TXData;


/* ————————————————————————————————————*/
// UART口判断
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

// UART模式判断
#if			UART_Mode_Tx
#define		UART_Mode_x		USART_Mode_Tx
#elif		UART_Mode_Rx
#define		UART_Mode_x		USART_Mode_Rx
#elif		UART_Mode_TxAndRx
#define		UART_Mode_x		(USART_Mode_Rx | USART_Mode_Tx)
#endif

// DMA通道判断
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
/* ————————————————————————*/


void PS2ToSlave1_Init(void)
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

	// USART口设置
		// 开启USART时钟
#if USART_1
	RCC_APB2PeriphClockCmd(RCC_UART, ENABLE);
#elif USART_2 || USART_3 || UART_4
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
	USART_Cmd(USART_x, ENABLE);								// 使能串口

	// DMA设置
		// 开启DMA时钟
#if UART_4
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);		// 开启DMA2的时钟
#elif USART_1 || USART_2 || USART_3
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		// 开启DMA1的时钟
#endif

	// DMA初始化
	DMA_InitTypeDef DMA_InitStructure;											// 定义结构体变量
		// TX
#if UART_Mode_Tx || UART_Mode_TxAndRx
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART_x->DR;			// 外设基地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		// 外设数据宽度，选择字节
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			// 外设地址自增，选择失能，始终以USART的DR寄存器为目的地
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART_TXData;				// 存储器基地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				// 存储器数据宽度，选择字节，与外设数据宽度对应
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						// 存储器地址自增，选择使能，每次转运后，数组移到下一个位置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;							// 数据传输方向，选择由存储器到外设，数组转到串口数据寄存器
	DMA_InitStructure.DMA_BufferSize = 0;										// 转运的数据大小（转运次数）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								// 模式，选择单次模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								// 存储器到存储器，选择失能
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						// 优先级，选择中等
	DMA_Init(TX_DMAx_Channely, &DMA_InitStructure);
#endif

		// RX
#if UART_Mode_Rx || UART_Mode_TxAndRx
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART_x->DR;			// 外设基地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		// 外设数据宽度，选择字节
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			// 外设地址自增，选择失能，始终以USART的DR寄存器为目的地
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART_RXData;				// 存储器基地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				// 存储器数据宽度，选择字节，与外设数据宽度对应
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						// 存储器地址自增，选择使能，每次转运后，数组移到下一个位置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							// 数据传输方向，选择由外设到存储器，串口数据寄存器转到数组
	DMA_InitStructure.DMA_BufferSize = 26;										// 转运的数据大小（转运次数）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								// 模式，选择循环模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								// 存储器到存储器，选择失能
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							// 优先级，选择高
	DMA_Init(RX_DMAx_Channely, &DMA_InitStructure);
#endif

	// DMA使能
		// TX
#if UART_Mode_Tx || UART_Mode_TxAndRx
	USART_DMACmd(USART_x, USART_DMAReq_Tx, ENABLE);			// 使能USART的TX的DMA转运
#endif

		// RX
#if UART_Mode_Rx || UART_Mode_TxAndRx
	DMA_Cmd(RX_DMAx_Channely, ENABLE);						// DMA使能
	USART_DMACmd(USART_x, USART_DMAReq_Rx, ENABLE);			// 使能USART的RX的DMA转运
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

	DMA_Cmd(TX_DMAx_Channely, DISABLE);					// 失能DMA，停止发送数据

	DMA_SetCurrDataCounter(TX_DMAx_Channely, 26);		// 设置传输长度

	DMA_Cmd(TX_DMAx_Channely, ENABLE);					// 使能DMA，开始发送数据
}

PS2_DataTypeDef* PS2FromMaster_GetPoint_PS2Data(void)
{
	return &PS2_RXData;
}

/**
  * @brief  读取来自主机的PS2数据
  * @param  无
  * @retval (int8_t)是否成功读取PS2数据，成功返回1，失败返回-1
  * @note	如果读取成功，则新的数据覆盖上一次数据
  * @note	如果读取失败，则保持上一次数据
  * @note	读取DMA转运储存数组时，DMA会被失能，停止接收新的数据，直到读取完毕重新使能
  * @note	在此期间，如果传入新的数据，将会无法接收而丢失
  * @note	换言之，DMA只有一重缓冲，要解决应该使用双重缓冲，但是我不想改了
  */
int8_t PS2FromMaster_ReadData(void)
{
	uint8_t DataPacket_DataLength = 0;
	uint8_t DataPacket_Data[22];
	uint8_t DataPacket_Sum = 0;

	// 这里遇到了和ATK_IMU901模块里面遇到的相似的问题
	// 即如果在程序中进行DMA失能，在接收串口数据时
	// 如果产生了断联时，有较大的概率会导致DMA转运失效，需要重新上电复位
	//DMA_Cmd(RX_DMAx_Channely, DISABLE);			// 失能DMA，停止接收数据

	uint8_t SuccessfullyReadFlagBit = 0;

	for (uint8_t i = 0; i < 26; i++)
	{
		if (UART_RXData[i] == 0x55)		// 判断帧头0x55
		{
			DataPacket_Sum += UART_RXData[i];

			i++;

			if (i == 26)	// 判断是否到达数组末尾
			{
				i = 0;
			}

			if (UART_RXData[i] == 0x40)		// 判断ID
			{
				DataPacket_Sum += UART_RXData[i];

				i++;

				if (i == 26)	// 判断是否到达数组末尾
				{
					i = 0;
				}

				DataPacket_DataLength = UART_RXData[i];

				for (uint8_t j = 0; j < DataPacket_DataLength; j++)
				{
					DataPacket_Sum += UART_RXData[i];

					i++;

					if (i == 26)	// 判断是否到达数组末尾
					{
						i = 0;
					}

					DataPacket_Data[j] = UART_RXData[i];

				}

				DataPacket_Sum += UART_RXData[i];

				i++;

				if (i == 26)	// 判断是否到达数组末尾
				{
					i = 0;
				}

				if (DataPacket_Sum != UART_RXData[i])
				{
					DMA_Cmd(RX_DMAx_Channely, ENABLE);			// 使能DMA，开始接收数据
					return -1;			// 读取失败，返回错误值-1
				}
				else
				{
					SuccessfullyReadFlagBit++;
					break;
				}
			}
		}
	}

	//DMA_Cmd(RX_DMAx_Channely, ENABLE);			// 使能DMA，开始接收数据

	if (!SuccessfullyReadFlagBit)
	{
		return -1;			// 读取失败，返回错误值-1
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

	return 1;		// 读取成功，返回正常值1
}

