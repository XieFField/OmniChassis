/*ע�⣺�ù��̵�stm32f10x.h�ļ��еĺ궨��stm32f10x_conf.h�Ѿ��򿪣�*/
/*ע�⣺��Ҫ�򿪹��̵�microLibѡ�*/

#include "stm32f10x.h"                  // Device header
#include "Incl_z.h"
#include "Chassis.h"
#include "bsp_motorTIM.h"


int main(void)
{
	// ���ߴ��ڲ���
	//MyUART_Init(115200);		// �������ߴ�������ƺ�֧��1382400�������Ҳ����˼��Ρ��ƺ������У������ٽ�һ��������
								// 921600�����������ӣ�ͨ�ţ����Ǵ������ֵĴ�ӡ�ٶȸ����ϣ������ӳٺܴ󣬲����������ʲôԭ��

    
    	VL5300_DataSetTypeDef* VL5300_DataSet;
	VL5300_DataSet = VL5300_GetDataSetPoint();

	OLED_Init();
	Chassis_Init();
	Delay_s(1);
	while (1)
	{
        VL5300_ReadData();
        
		Chassis_FSM_TEST();
        
        OLED_Show_u32(1, 1, (uint16_t)VL5300_DataSet->Front.Correction_TOF, 4);
        OLED_Show_u32(2, 1, (uint16_t)VL5300_DataSet->Left.Correction_TOF, 4);
        OLED_Show_u32(3, 1, (uint16_t)VL5300_DataSet->Right.Correction_TOF, 4);
	}
}
