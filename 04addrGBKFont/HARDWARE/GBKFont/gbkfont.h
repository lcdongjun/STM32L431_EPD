#ifndef __GBKFONT_H
#define __GBKFONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

#define MAX_GBK_CHAR 0xFFFF
#define FONT_INDEX_AREA_ADDR 0x10000
#define FONT_16x16_AREA_ADDR 0x20000   // 16x16 �ֿ���������
#define FONT_24x24_AREA_ADDR 0x30000   // 24x24 �ֿ���������
#define FONT_48x48_AREA_ADDR 0x40000   // 48x48 �ֿ���������

__packed typedef struct 
{
	u8 fontok;				//�ֿ���ڱ�־��0XAA���ֿ��������������ֿⲻ����
	u32 f12addr;			//gbk12��ַ	
	u32 gbk12size;			//gbk12�Ĵ�С	 
	u32 f16addr;			//gbk16��ַ
	u32 gbk16size;			//gbk16�Ĵ�С		 
	u32 f24addr;			//gbk24��ַ
	u32 gbk24size;			//gbk24�Ĵ�С 
	u32 f48addr;			//gbk48��ַ
	u32 gbk48size;			//gbk48�Ĵ�С 
}_font_info; 

extern _font_info ftinfo;	//�ֿ���Ϣ�ṹ��


void updata_fontx(void);
u8 font_init(void);
#ifdef __cplusplus
}
#endif

#endif /* __GBKFONT_H */

