#ifndef __GBKFONT_H
#define __GBKFONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

#define MAX_GBK_CHAR 0xFFFF
#define FONT_INDEX_AREA_ADDR 0x10000
#define FONT_16x16_AREA_ADDR 0x20000   // 16x16 字库数据区域
#define FONT_24x24_AREA_ADDR 0x30000   // 24x24 字库数据区域
#define FONT_48x48_AREA_ADDR 0x40000   // 48x48 字库数据区域

__packed typedef struct 
{
	u8 fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
	u32 f12addr;			//gbk12地址	
	u32 gbk12size;			//gbk12的大小	 
	u32 f16addr;			//gbk16地址
	u32 gbk16size;			//gbk16的大小		 
	u32 f24addr;			//gbk24地址
	u32 gbk24size;			//gbk24的大小 
	u32 f48addr;			//gbk48地址
	u32 gbk48size;			//gbk48的大小 
}_font_info; 

extern _font_info ftinfo;	//字库信息结构体


void updata_fontx(void);
u8 font_init(void);
#ifdef __cplusplus
}
#endif

#endif /* __GBKFONT_H */

