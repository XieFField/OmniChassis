/*注意：该工程的stm32f10x.h文件中的宏定义stm32f10x_conf.h已经打开！*/
/*注意：需要打开工程的microLib选项！*/

#include "stm32f10x.h"                  // Device header
#include "Incl_z.h"
#include "Chassis.h"
#include "bsp_motorTIM.h"


int main(void)
{
	Delay_ms(10);
	
//	OLED_Demo();
	
//	VL5300_Demo();

    // PS2_Demo();
//    PS2ToSlave1_Demo();
    
    
//    ATK_IMU901_Demo();
	// 无线串口测试
	//MyUART_Init(115200);		// 蓝牙无线串口最高似乎支持1382400，但是我测试了几次“似乎”不行，不想再进一步测试了
								// 921600可以正常连接，通信，但是串口助手的打印速度跟不上，反而延迟很大，不清楚具体是什么原因
    

//    OLED_Demo();
//	printf("Start\r\n");
	Chassis_Init();
	Delay_s(1);

 
	PS2_Init();

	while (1)
	{
		// Chassis_Task();
		Chassis_FSM_TEST();
	}
}
