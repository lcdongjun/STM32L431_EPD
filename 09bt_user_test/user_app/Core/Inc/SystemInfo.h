#ifndef __SYSTEMINFO_H
#define __SYSTEMINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"
#include "aht20.h"
#include "DS3231.h"



extern SemaphoreHandle_t xSemaphoreUSART1;   // 用于保护USART1的同步

extern TaskHandle_t xReceiveTaskHandle;			 //USART接收数据 and AT指令任务


extern TaskHandle_t ADC_Value_Handle;				 //测试ADC采集电池电压任务
extern TaskHandle_t EPD_test_Handle;				 //测试墨水屏墨水屏任务
extern TaskHandle_t EXP_test_Handle;				 //测试温湿度采集 and 时间获取任务


extern SemaphoreHandle_t rtcAlarmASemaphore; //RTC闹钟通知刷新时间

extern TaskHandle_t RTC_AlarmATask_Handle;	 //RTC闹钟A显示时间
extern TaskHandle_t TimeDisplayTask_Handle;	 //正常显示时间


typedef struct {
    // 系统设置
    struct {
        uint32_t lockScreenTimeout;  // 锁屏时间 (单位：秒)
        uint8_t buzzerEnabled;       // 蜂鸣器设置 (0: 关闭, 1: 开启)
    } SystemSettings;

    // 天气信息
    struct {
        float temperature;           // 温度 (单位：摄氏度)
        float humidity;              // 湿度 (%)
        char weather[32];            // 天气描述 (如“晴”，“多云”等)
				AHT20_Data aht20_data;
    } Weather;

    // 时间信息
		DateTime DT;

    // 系统状态
    struct {
        uint8_t batteryLevel;       // 电池电量 (0~100%)
        uint8_t bluetoothConnected; // 蓝牙连接状态 (0: 未连接, 1: 已连接)
    } SystemStatus;
    
} SystemInfo_t;
extern SystemInfo_t SystemInfo;


void Sync_time(void);
void Sync_AHT20(void);
void Sync_BatteryLevel(void);
void SystemInfoInit(void);



#ifdef __cplusplus
}
#endif



#endif /* __SYSTEMINFO_H */
