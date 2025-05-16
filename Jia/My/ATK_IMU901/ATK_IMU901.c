/**
 * @platform	keil5平台STM32F103RCT6型号单片机
 * @mainpage	机器人学院十期基础赛第三组R1夹球车主单片机(型号F103RCT6)
 * @author		桑叁
 * @date		2025年4月14日
 */


// 采用硬件USART进行串口通讯
// 使用DMA进行数据转运，节约运算资源
// 只写了姿态角和陀螺仪、加速度计的数据读取
// 详细配置情况参考同目录下保存的图片
// 目前代码中，陀螺仪的实际有效输出频率约为：上报频率/3


#include <stdint.h>
#include "stm32f10x.h"
#include "Delay_API_z.h"
#include "ATK_IMU901_API.h"


#define ATK_IMU901_bound		921600


// 设置陀螺仪（单位：度/s）和加速度计（单位：G）的量程
#define AccelerationRange		2
#define GyroscopeRange 			1000


// USRT通讯最大一次性接收字节数
#define USART_REC_LEN		80			// USART_REC_LEN 的全称可能是 USART Receive Length
										// 数据长度（帧格式里的数据位+帧格式里的其他位）：
										// 1. 姿态角 			6+5个字节
										// 2. 四元数 			8+5个字节
										// 3. 陀螺仪和加速度计 	12+5个字节
										// 4. 磁力计			8+5个字节			上报频率固定20Hz
										// 5. 气压计			10+5个字节			上报频率固定20Hz
										// 6. 端口状态			8+5个字节


/* ————————————————————————————————————*/
// GPIO口初始化
#define GPIO_Speed_x		GPIO_Speed_50MHz

#define RCC_TX				RCC_APB2Periph_GPIOC		// TX
#define GPIOx_TX			GPIOC
#define GPIO_Pinx_TX		GPIO_Pin_10

#define RCC_RX				RCC_APB2Periph_GPIOC		// RX
#define GPIOx_RX			GPIOC
#define GPIO_Pinx_RX		GPIO_Pin_11
/* ————————————————————————*/


/* ————————————————————————————————————*/
// UART口初始化
// UART口选择，启用为1，关闭为0，最多启用一个！
// UART5没有DMA转运功能！所以这里禁止使用UART5！
#define USART_1				0			// 避免与 USARTx 宏定义重名
#define USART_2				0			// 避免与 USARTx 宏定义重名
#define USART_3				0			// 避免与 USARTx 宏定义重名
#define UART_4				1			// 避免与 UARTx 宏定义重名
//#define UART_5				0			// 避免与 UARTx 宏定义重名

// UART口收发模式选择，选择三个中的一个，Tx是只发模式，Rx是只收模式，TxAndRx是收发模式，最多启用一个！
#define	UART_Mode_Tx				0	// 避免与 USART_Mode_Tx 宏定义重名
#define	UART_Mode_Rx				1	// 避免与 USART_Mode_Rx 宏定义重名
#define	UART_Mode_TxAndRx			0
/* ————————————————————————*/


static ATK_IMU901_DataTypeDef ATK_IMU901_Data;


/* ————————————————————————————————————*/
// UART口判断
#if USART_1
//#define IRQn_USARTx 			USART1_IRQn
#define RCC_UART 				RCC_APB2Periph_USART1
#define USART_x 				USART1
//#define USARTx_IRQHandler 		USART1_IRQHandler
#elif USART_2
//#define IRQn_USARTx 			USART2_IRQn
#define RCC_UART 				RCC_APB1Periph_USART2
#define USART_x 				USART2
//#define USARTx_IRQHandler 		USART2_IRQHandler
#elif USART_3
//#define IRQn_USARTx 			USART3_IRQn
#define RCC_UART 				RCC_APB1Periph_USART3
#define USART_x 				USART3
//#define USARTx_IRQHandler 		USART3_IRQHandler
#elif UART_4
//#define IRQn_USARTx 			UART4_IRQn
#define RCC_UART 				RCC_APB1Periph_UART4
#define USART_x 				UART4
//#define USARTx_IRQHandler 		UART4_IRQHandler
//#elif UART_5
////#define IRQn_USARTx 			UART5_IRQn
//#define RCC_UART 				RCC_APB1Periph_UART5
//#define USART_x 				UART5
////#define USARTx_IRQHandler 		UART5_IRQHandler
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
#if UART_Mode_Rx || UART_Mode_TxAndRx
#if USART_1
#define RX_DMAx_Channely	DMA1_Channel5
#elif USART_2
#define RX_DMAx_Channely	DMA1_Channel6
#elif USART_3
#define RX_DMAx_Channely	DMA1_Channel3
#elif UART_4
#define RX_DMAx_Channely	DMA2_Channel3
#endif
#endif
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


static uint8_t ReceiveDataString[USART_REC_LEN];


// 用户手册中提到：
// 角度精度：X、Y轴：静态0.05°，动态0.1°，Z轴：0.5°
// 数据输出速率1-250Hz


// 实测数据（需要根据每个芯片的情况单独测定）
// 在陀螺仪量程1000度/s，加速度计2g/s的量程下
// 传感器反向插接在平放在桌面的夹球车电路板上
// 加速度计X、Y、Z轴的10次平均数据为0.0000g、0.0148g、-0.9726g
// 角速度计X、Y、Z轴的5次平均数据为0.3967、0.2879、0.5898


void ATK_IMU901_Init()
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
//#if UART_Mode_Tx || UART_Mode_TxAndRx
//	TX_H;
//#endif

	// USART口设置
		// 开启USART时钟
#if USART_1
	RCC_APB2PeriphClockCmd(RCC_UART, ENABLE);
#elif USART_2 || USART_3 || UART_4 || UART_5
	RCC_APB1PeriphClockCmd(RCC_UART, ENABLE);
#endif

	// USART初始化
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = ATK_IMU901_bound;							// 串口波特率
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
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ReceiveDataString;			// 存储器基地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				// 存储器数据宽度，选择字节，与外设数据宽度对应
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						// 存储器地址自增，选择使能，每次转运后，数组移到下一个位置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							// 数据传输方向，选择由外设到存储器，串口数据寄存器转到数组
	DMA_InitStructure.DMA_BufferSize = USART_REC_LEN;							// 转运的数据大小（转运次数）
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

//void USARTx_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART_x, USART_IT_RXNE) == SET)		// 判断中断
//	{
//		
//	}
//
//	USART_ClearFlag(USART_x, USART_IT_RXNE);					// 清空中断标志位
//}

ATK_IMU901_DataTypeDef* ATK_IMU901_GetPoint_Data(void)
{
	return &ATK_IMU901_Data;
}

/**
  * @brief  读取ATK_IMU901数据
  * @param  无
  * @retval 无
  */
void ATK_IMU901_ReadData(void)
{
	uint8_t SUM;

	/* 我不知道为什么这里不能在读取DMA直接存取的数组的时候失能DMA转运，会导致上电后一段时间程序失效 */
	//DMA_Cmd(RX_DMAx_Channely, DISABLE);					// DMA2的通道3失能
	//USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE);		// 失能USART的DMA转运

	for (uint8_t i1 = 0; i1 < (USART_REC_LEN - 20);)
	{
		SUM = 0;

		if (ReceiveDataString[i1++] == 0x55)							// 判断ATK-IMU901的帧格式的第1个字节
		{
			if (ReceiveDataString[i1++] == 0x55);					// 判断ATK-IMU901的帧格式的第2个字节
			{
				switch (ReceiveDataString[i1++])						// 判断ATK-IMU901的帧格式的第3个字节
				{
				case 0x01:
					if (ReceiveDataString[i1++] == 6)				// 判断ATK-IMU901的帧格式的第4个字节
					{
						SUM += 0x55 + 0x55 + 0x01 + 6;
						for (uint8_t j = 0; j < 6; j++)
							SUM += ReceiveDataString[i1 + j];

						if (SUM == ReceiveDataString[i1 + 6])		// 判断ATK-IMU901的帧格式的SUM字节
						{
							// 开始处理ATK-IMU901的帧格式的DATA段数据
							uint8_t DataH, DataL;						// 定义数据高8位和低8位的变量

							// 读Roll
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.Roll = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * 180;

							// 读Pitch
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.Pitch = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * 180;

							// 读Yaw
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.Yaw = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * 180;

							// 跳过SUM字节，因为先前已经处理过
							i1++;
						}
					}
					else
						i1 += 7;
					break;
				case 0x03:
					if (ReceiveDataString[i1++] == 12)				// 判断ATK-IMU901的帧格式的第4个字节
					{
						SUM += 0x55 + 0x55 + 0x03 + 12;
						for (uint8_t j = 0; j < 12; j++)
							SUM += ReceiveDataString[i1 + j];

						if (SUM == ReceiveDataString[i1 + 12])		// 判断ATK-IMU901的帧格式的SUM字节
						{
							// 开始处理ATK-IMU901的帧格式的DATA段数据
							uint8_t DataH, DataL;						// 定义数据高8位和低8位的变量

							// 读加速度计X轴
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.AccX = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;

							// 读加速度计Y轴
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.AccY = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;

							// 读加速度计Z轴
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.AccZ = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * AccelerationRange;

							// 读陀螺仪X轴
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.GyroX = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;

							// 读陀螺仪Y轴
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.GyroY = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;

							// 读陀螺仪Z轴
							DataL = ReceiveDataString[i1++];
							DataH = ReceiveDataString[i1++];
							ATK_IMU901_Data.GyroZ = (float)((int16_t)((DataH << 8) | DataL)) / 32768 * GyroscopeRange;

							// 跳过SUM字节，因为先前已经处理过
							i1++;
						}
					}
					else
						i1 += 13;
					break;
				}
			}
		}
	}

	//DMA_Cmd(RX_DMAx_Channely, ENABLE);							// DMA2的通道3使能
	//USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);			// 使能USART的DMA转运
}

