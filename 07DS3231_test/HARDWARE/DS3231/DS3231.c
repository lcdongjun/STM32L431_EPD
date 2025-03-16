#include "DS3231.h"


bool DS3231_Init(void) {
    // ��ʼ��GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin = T_SCL_Pin;
    HAL_GPIO_Init(T_SCL_GPIO_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = T_SDA_Pin;
    HAL_GPIO_Init(T_SDA_GPIO_Port, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET);
		Delay_us(20000);
    return 1;
}

static void I2C_DELAY()
{
	Delay_us(100);
}

// ���I2C��������
static void I2C_Start(void) {
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_RESET);
    I2C_DELAY();
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_RESET);
    I2C_DELAY();
}

static void I2C_Stop(void) {
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET);
    I2C_DELAY();
}

static bool I2C_WriteByte(uint8_t byte) {
    for(uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, (byte & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        byte <<= 1;
        HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
        I2C_DELAY();
        HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_RESET);
        I2C_DELAY();
    }

    // ���ACK
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
    I2C_DELAY();
    bool ack = (HAL_GPIO_ReadPin(T_SDA_GPIO_Port, T_SDA_Pin) == GPIO_PIN_RESET);
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_RESET);
    I2C_DELAY();
    return ack;
}

static bool I2C_ReadByte(uint8_t *byte, bool ack) {
    uint8_t data = 0;
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET);
    
    for(uint8_t i = 0; i < 8; i++) {
        data <<= 1;
        HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
        I2C_DELAY();
        if(HAL_GPIO_ReadPin(T_SDA_GPIO_Port, T_SDA_Pin)) data |= 0x01;
        HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_RESET);
        I2C_DELAY();
    }

    // ����ACK/NACK
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, ack ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_RESET);
    I2C_DELAY();
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET);
    
    *byte = data;
    return true;
}


/* BCDת�����ߺ��� */
static inline uint8_t bcd_to_dec(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static inline uint8_t dec_to_bcd(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}



/* �Ĵ�����д�ײ㺯�� */
static bool DS3231_ReadRegister(uint8_t reg, uint8_t *value) {
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_WRITE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(reg)) { I2C_Stop(); return false; }
    
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_READ)) { I2C_Stop(); return false; }
    bool ret = I2C_ReadByte(value, false);
    I2C_Stop();
    return ret;
}

static bool DS3231_WriteRegister(uint8_t reg, uint8_t value) {
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_WRITE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(reg)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(value)) { I2C_Stop(); return false; }
    I2C_Stop();
    return true;
}

/* ʱ���д���ĺ��� */
bool DS3231_GetTime(DateTime *dt) {
    uint8_t data[7];
    
    // ������ȡ7��ʱ��Ĵ���
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_WRITE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(DS3231_TIME_REG)) { I2C_Stop(); return false; }
    
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_READ)) { I2C_Stop(); return false; }
    
    for(int i=0; i<7; i++) {
        I2C_ReadByte(&data[i], (i == 6) ? false : true);
    }
    I2C_Stop();

    // ����ԭʼ����
    dt->second = bcd_to_dec(data[0] & 0x7F); // ����ʱ��ֹͣλ
    dt->minute = bcd_to_dec(data[1]);
    dt->hour = bcd_to_dec(data[2] & 0x3F); // 24Сʱ�ƴ���
    dt->dayOfWeek = bcd_to_dec(data[3]);
    dt->dayofmonth = bcd_to_dec(data[4]);
    
    // ��������λ
    uint8_t century = (data[5] & 0x80) ? 1 : 0;
    dt->month = bcd_to_dec(data[5] & 0x1F);
    dt->year = 2000 + century*100 + bcd_to_dec(data[6]);

    return true;
}

bool DS3231_SetTime(DateTime *dt) {
    uint8_t data[7];
    
    // ת��ʱ�䵽�Ĵ�����ʽ
    data[0] = dec_to_bcd(dt->second) & 0x7F; // ȷ��ʱ������
    data[1] = dec_to_bcd(dt->minute);
    data[2] = dec_to_bcd(dt->hour);      // Ĭ��24Сʱ��
    data[3] = dec_to_bcd(dt->dayOfWeek);
    data[4] = dec_to_bcd(dt->dayofmonth);
    
    // ��������λ
    uint8_t century = (dt->year >= 2000) ? 1 : 0;
    data[5] = dec_to_bcd(dt->month) | (century << 7);
    data[6] = dec_to_bcd(dt->year % 100);

    // ����д��Ĵ���
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_WRITE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(DS3231_TIME_REG)) { I2C_Stop(); return false; }
    
    for(int i=0; i<7; i++) {
        if(!I2C_WriteByte(data[i])) { I2C_Stop(); return false; }
    }
    I2C_Stop();
    
    return true;
}

bool DS3231_SetSQW(SQW_Freq freq) {
    uint8_t ctrl;
    if(!DS3231_ReadRegister(DS3231_CONTROL_REG, &ctrl)) return false;
    // ͳһ����������λ
    ctrl &= ~(DS3231_RS1 | DS3231_RS2 | DS3231_INTCN | DS3231_BBSQW);
    switch(freq) {
    case SQW_DISABLE:    // �ر�SQW���
        ctrl |= DS3231_INTCN;  // �����ж�ģʽ
        break;
        
    case SQW_1HZ:       // 1Hz����
        ctrl &= ~DS3231_INTCN; // ���÷������
        break;
        
    case SQW_1024HZ:    // 1024Hz
        ctrl |= DS3231_RS1;
        ctrl &= ~DS3231_INTCN;
        break;
        
    case SQW_4096HZ:    // 4096Hz
        ctrl |= DS3231_RS2;
        ctrl &= ~DS3231_INTCN;
        break;
        
    case SQW_8192HZ:    // 8192Hz
        ctrl |= (DS3231_RS1 | DS3231_RS2);
        ctrl &= ~DS3231_INTCN;
        break;
        
    default:
        return false;
    }
    if(!DS3231_WriteRegister(DS3231_CONTROL_REG, ctrl)) return false;
    uint8_t verify;
    DS3231_ReadRegister(DS3231_CONTROL_REG, &verify);
    return (verify == ctrl);
}


/* 32kHz������� */
bool DS3231_Enable32kHz(bool enable) {
    uint8_t status;
    if(!DS3231_ReadRegister(DS3231_STATUS_REG, &status)) return false;
    
    status = enable ? (status | DS3231_EN32KHZ) : (status & ~DS3231_EN32KHZ);
    
    return DS3231_WriteRegister(DS3231_STATUS_REG, status);
}

bool DS3231_SetAlarm1(const AlarmConfig *config) {
    uint8_t data[4];
    

    data[0] = dec_to_bcd(config->seconds) | ((config->mode >= ALARM_MATCH_MIN_SEC) ? 0x00 : 0x80);
    data[1] = dec_to_bcd(config->minutes) | ((config->mode >= ALARM_MATCH_HOUR_MIN_SEC) ? 0x00 : 0x80);
    data[2] = dec_to_bcd(config->hours) | ((config->mode >= ALARM_MATCH_DATE_HOUR_MIN_SEC) ? 0x00 : 0x80);
    

    if(config->mode == ALARM_MATCH_DAY_HOUR_MIN_SEC) {
        data[3] = dec_to_bcd(config->day_or_date) | 0x40;
    } else {
        data[3] = dec_to_bcd(config->day_or_date) | ((config->mode >= ALARM_MATCH_DAY_HOUR_MIN_SEC) ? 0x00 : 0x80);
    }
		
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_WRITE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(DS3231_ALARM1_REG)) { I2C_Stop(); return false; }
    for(int i=0; i<4; i++) {
        if(!I2C_WriteByte(data[i])) { I2C_Stop(); return false; }
    }
    I2C_Stop();
    uint8_t ctrl;
    if(!DS3231_ReadRegister(DS3231_CONTROL_REG, &ctrl)) return false;
    ctrl |= (1 << 0);
    return DS3231_WriteRegister(DS3231_CONTROL_REG, ctrl);
}
/* �жϴ�����غ��� */
// ������ӱ�־λ
bool DS3231_ClearAlarmFlag(void) {
    uint8_t status;
    if(!DS3231_ReadRegister(DS3231_STATUS_REG, &status)) return false;
    status &= ~(1 << 0); // ���A1F��־
    return DS3231_WriteRegister(DS3231_STATUS_REG, status);
}
// ������Ӵ���״̬
bool DS3231_CheckAlarmTriggered(void) {
    uint8_t status;
    if(!DS3231_ReadRegister(DS3231_STATUS_REG, &status)) return false;
    return (status & (1 << 0)) != 0;
}

static const char* weekday_str[] = {
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};

void PrintDateTime(const DateTime *dt) {
    // ��������Ϣ�Ŀɶ���ʽ
    printf("[Formatted] %s %02hu/%02hu/%04hu %02hu:%02hu:%02hu\n", 
           weekday_str[dt->dayOfWeek-1],  // ����Ĵ���ֵ1��Ӧ����
           dt->month,
           dt->dayofmonth,
           dt->year,
           dt->hour,
           dt->minute,
           dt->second);
}




