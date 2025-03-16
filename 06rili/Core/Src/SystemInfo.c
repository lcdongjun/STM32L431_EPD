#include "SystemInfo.h"
#include "rtc.h"
#include "adc.h"

typedef struct {
    float voltage;
    float soc;
} VoltageToSOC;

VoltageToSOC soc_table[] = {
    {4.20, 100},
    {4.10, 95},
    {4.00, 90},
    {3.90, 80},
    {3.80, 70},
    {3.70, 60},
    {3.60, 50},
    {3.50, 40},
    {3.40, 30},
    {3.30, 20},
    {3.20, 10},
    {3.00, 0}
};
#define TABLE_SIZE (sizeof(soc_table) / sizeof(VoltageToSOC))
#define SAMPLE_COUNT 10 //ADC采样次数

SystemInfo_t SystemInfo;

void Sync_time()
{
		RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
		
		SystemInfo.timeInfo.date = sDate;
		SystemInfo.timeInfo.time = sTime;
}

void Sync_AHT20()
{

	AHT20_ReadData(&SystemInfo.weather.aht20_data);
	
}

int compare(const void *a, const void *b) {
    return (*(uint32_t *)a - *(uint32_t *)b);
}

uint32_t adc_filtered_value(ADC_HandleTypeDef *hadc) {
    uint32_t samples[SAMPLE_COUNT];
    uint32_t sum = 0;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        HAL_ADC_PollForConversion(hadc, 50);
        if (HAL_IS_BIT_SET(HAL_ADC_GetState(hadc), HAL_ADC_STATE_REG_EOC)) {
            samples[i] = HAL_ADC_GetValue(hadc);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    qsort(samples, SAMPLE_COUNT, sizeof(uint32_t), compare);
    for (int i = 1; i < SAMPLE_COUNT - 1; i++) {
        sum += samples[i];
    }
    return sum / (SAMPLE_COUNT - 2);
}


float calculate_soc(float voltage) {
    if (voltage >= soc_table[0].voltage) {
        return soc_table[0].soc;
    }
    if (voltage <= soc_table[TABLE_SIZE - 1].voltage) {
        return soc_table[TABLE_SIZE - 1].soc;
    }

    for (int i = 0; i < TABLE_SIZE - 1; i++) {
        if (voltage >= soc_table[i + 1].voltage && voltage <= soc_table[i].voltage) {
            float slope = (soc_table[i].soc - soc_table[i + 1].soc) / (soc_table[i].voltage - soc_table[i + 1].voltage);
            return soc_table[i].soc + slope * (voltage - soc_table[i].voltage);
        }
    }

    return 0;
}


void Sync_BatteryLevel()
{
		uint16_t ADC_Value;
		float BatteryLevel;
		HAL_ADC_Stop(&hadc1);
		HAL_GPIO_WritePin(EN_CHK_VU_GPIO_Port, EN_CHK_VU_Pin, GPIO_PIN_SET);
		HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);
		vTaskDelay(pdMS_TO_TICKS(1000));
		HAL_ADC_Start(&hadc1);
		ADC_Value = adc_filtered_value(&hadc1);
		BatteryLevel =calculate_soc(ADC_Value * 2.1f / 4096 * 2);
		SystemInfo.systemStatus.batteryLevel = (uint8_t)BatteryLevel;
		HAL_GPIO_WritePin(EN_CHK_VU_GPIO_Port, EN_CHK_VU_Pin, GPIO_PIN_RESET);
		HAL_ADC_Stop(&hadc1);
}


void SystemInfoInit()
{
	
	Sync_time();
	Sync_AHT20();
	Sync_BatteryLevel();
}

