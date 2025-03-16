#ifndef __SYSTEMINFO_H
#define __SYSTEMINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"
#include "aht20.h"
#include "DS3231.h"



extern SemaphoreHandle_t xSemaphoreUSART1;   // ���ڱ���USART1��ͬ��

extern TaskHandle_t xReceiveTaskHandle;			 //USART�������� and ATָ������


extern TaskHandle_t ADC_Value_Handle;				 //����ADC�ɼ���ص�ѹ����
extern TaskHandle_t EPD_test_Handle;				 //����īˮ��īˮ������
extern TaskHandle_t EXP_test_Handle;				 //������ʪ�Ȳɼ� and ʱ���ȡ����


extern SemaphoreHandle_t rtcAlarmASemaphore; //RTC����֪ͨˢ��ʱ��

extern TaskHandle_t RTC_AlarmATask_Handle;	 //RTC����A��ʾʱ��
extern TaskHandle_t TimeDisplayTask_Handle;	 //������ʾʱ��


typedef struct {
    // ϵͳ����
    struct {
        uint32_t lockScreenTimeout;  // ����ʱ�� (��λ����)
        uint8_t buzzerEnabled;       // ���������� (0: �ر�, 1: ����)
    } SystemSettings;

    // ������Ϣ
    struct {
        float temperature;           // �¶� (��λ�����϶�)
        float humidity;              // ʪ�� (%)
        char weather[32];            // �������� (�硰�硱�������ơ���)
				AHT20_Data aht20_data;
    } Weather;

    // ʱ����Ϣ
		DateTime DT;

    // ϵͳ״̬
    struct {
        uint8_t batteryLevel;       // ��ص��� (0~100%)
        uint8_t bluetoothConnected; // ��������״̬ (0: δ����, 1: ������)
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
