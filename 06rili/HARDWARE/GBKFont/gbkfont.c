#include "gbkfont.h"
#include "cmsis_os.h"
#include "w25qxx.h" 


_font_info ftinfo;




void updata_fontx()
{
		u32 flashaddr=0;	
		ftinfo.fontok = 0x99; //Font OK
		ftinfo.f12addr=FONTINFOADDR+sizeof(ftinfo);	
		ftinfo.gbk12size=574560 ;			//GBK12��С
		flashaddr=ftinfo.f12addr;
		printf("GBK12 flashaddr:%#X\r\n",flashaddr);
	
		ftinfo.f16addr = ftinfo.f12addr + ftinfo.gbk12size;
		ftinfo.gbk16size = 261698; // GBK16 ��С
		flashaddr = ftinfo.f16addr;
		printf("GBK16 flashaddr:%#X\r\n", flashaddr);

		ftinfo.f24addr = ftinfo.f16addr + ftinfo.gbk16size;
		ftinfo.gbk24size = 588818; // GBK24 ��С
		flashaddr = ftinfo.f24addr;
		printf("GBK24 flashaddr:%#X\r\n", flashaddr);

		ftinfo.f48addr = ftinfo.f24addr + ftinfo.gbk24size;
		ftinfo.gbk48size = 2355266 ; // GBK48 ��С
		flashaddr = ftinfo.f48addr;
		printf("GBK48 flashaddr:%#X\r\n", flashaddr);
	
		W25QXX_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));
}

u8 font_init(void)
{		
	u8 t=0;
//	W25QXX_Init();  
	while(t<10)//������ȡ10��,���Ǵ���,˵��ȷʵ��������,�ø����ֿ���
	{
		t++;
		W25QXX_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//����ftinfo�ṹ������
		if(ftinfo.fontok==0X99)
		{
				printf("ftinfo OK\r\n");
				break;
		}
		HAL_Delay(20);
	}
	if(ftinfo.fontok!=0X99)return 1;
	return 0;		    
}
