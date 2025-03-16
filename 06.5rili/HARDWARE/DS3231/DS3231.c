#include "DS3231.h"


bool DS3231_Init(void) {
    // 初始化GPIO
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
		vTaskDelay(pdMS_TO_TICKS(20));
    return 1;
}

static void I2C_DELAY()
{
	vTaskDelay(pdMS_TO_TICKS(1));
}

// 软件I2C基础函数
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

    // 检查ACK
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

    // 发送ACK/NACK
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, ack ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_RESET);
    I2C_DELAY();
    HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET);
    
    *byte = data;
    return true;
}


/* BCD转换工具函数 */
static inline uint8_t bcd_to_dec(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static inline uint8_t dec_to_bcd(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}



/* 寄存器读写底层函数 */
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

/* 时间读写核心函数 */
bool DS3231_GetTime(DateTime *dt) {
    uint8_t data[7];
    
    // 连续读取7个时间寄存器
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_WRITE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(DS3231_TIME_REG)) { I2C_Stop(); return false; }
    
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_READ)) { I2C_Stop(); return false; }
    
    for(int i=0; i<7; i++) {
        I2C_ReadByte(&data[i], (i == 6) ? false : true);
    }
    I2C_Stop();

    // 解析原始数据
    dt->second = bcd_to_dec(data[0] & 0x7F); // 忽略时钟停止位
    dt->minute = bcd_to_dec(data[1]);
    dt->hour = bcd_to_dec(data[2] & 0x3F); // 24小时制处理
    dt->dayOfWeek = bcd_to_dec(data[3]);
    dt->dayofmonth = bcd_to_dec(data[4]);
    
    // 处理世纪位
    uint8_t century = (data[5] & 0x80) ? 1 : 0;
    dt->month = bcd_to_dec(data[5] & 0x1F);
    dt->year = 2000 + century*100 + bcd_to_dec(data[6]);

    return true;
}

bool DS3231_SetTime(DateTime *dt) {
    uint8_t data[7];
    
    // 转换时间到寄存器格式
    data[0] = dec_to_bcd(dt->second) & 0x7F; // 确保时钟运行
    data[1] = dec_to_bcd(dt->minute);
    data[2] = dec_to_bcd(dt->hour);      // 默认24小时制
    data[3] = dec_to_bcd(dt->dayOfWeek);
    data[4] = dec_to_bcd(dt->dayofmonth);
    
    // 处理世纪位
    uint8_t century = (dt->year >= 2000) ? 1 : 0;
    data[5] = dec_to_bcd(dt->month) | (century << 7);
    data[6] = dec_to_bcd(dt->year % 100);

    // 批量写入寄存器
    I2C_Start();
    if(!I2C_WriteByte(DS3231_ADDR_WRITE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(DS3231_TIME_REG)) { I2C_Stop(); return false; }
    
    for(int i=0; i<7; i++) {
        if(!I2C_WriteByte(data[i])) { I2C_Stop(); return false; }
    }
    I2C_Stop();
    
    return true;
}

/* SQW输出控制 */
bool DS3231_SetSQW(SQW_Freq freq) {
    uint8_t ctrl;
    if(!DS3231_ReadRegister(DS3231_CONTROL_REG, &ctrl)) return false;
    
    // 清除频率设置位
    ctrl &= ~(DS3231_RS1 | DS3231_RS2);
    
    switch(freq) {
        case SQW_1HZ:    break; // 00
        case SQW_1024HZ: ctrl |= DS3231_RS1; break;
        case SQW_4096HZ: ctrl |= DS3231_RS2; break;
        case SQW_8192HZ: ctrl |= (DS3231_RS1 | DS3231_RS2); break;
        default: return false;
    }
    
    // 启用SQW输出
    ctrl &= ~DS3231_INTCN;
    
    return DS3231_WriteRegister(DS3231_CONTROL_REG, ctrl);
}

/* 32kHz输出控制 */
bool DS3231_Enable32kHz(bool enable) {
    uint8_t status;
    if(!DS3231_ReadRegister(DS3231_STATUS_REG, &status)) return false;
    
    status = enable ? (status | DS3231_EN32KHZ) : (status & ~DS3231_EN32KHZ);
    
    return DS3231_WriteRegister(DS3231_STATUS_REG, status);
}





