/*ע�⣺�ù��̵�stm32f10x.h�ļ��еĺ궨��stm32f10x_conf.h�Ѿ��򿪣�*/
/*ע�⣺��Ҫ�򿪹��̵�microLibѡ�*/

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
	// ���ߴ��ڲ���
	//MyUART_Init(115200);		// �������ߴ�������ƺ�֧��1382400�������Ҳ����˼��Ρ��ƺ������У������ٽ�һ��������
								// 921600�����������ӣ�ͨ�ţ����Ǵ������ֵĴ�ӡ�ٶȸ����ϣ������ӳٺܴ󣬲����������ʲôԭ��
    

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
