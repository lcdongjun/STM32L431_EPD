include "DS1307.h"


// 软件I2C时序控制
#define I2C_DELAY() Delay_us(30)// 根据实际时钟速度调整延时

// GPIO操作宏
#define SCL_HIGH()  HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_SET)
#define SCL_LOW()   HAL_GPIO_WritePin(T_SCL_GPIO_Port, T_SCL_Pin, GPIO_PIN_RESET)
#define SDA_HIGH()  HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_SET)
#define SDA_LOW()   HAL_GPIO_WritePin(T_SDA_GPIO_Port, T_SDA_Pin, GPIO_PIN_RESET)
#define SDA_READ()  HAL_GPIO_ReadPin(T_SDA_GPIO_Port, T_SDA_Pin)

/* 添加软件I2C基础函数 */
static void i2c_start(void) {
    SDA_HIGH();
    SCL_HIGH();
    I2C_DELAY();
    SDA_LOW();
    I2C_DELAY();
    SCL_LOW();
}

static void i2c_stop(void) {
    SCL_LOW();
    SDA_LOW();
    I2C_DELAY();
    SCL_HIGH();
    I2C_DELAY();
    SDA_HIGH();
    I2C_DELAY();
}

static uint8_t i2c_write_byte(uint8_t byte) {
    for(uint8_t i = 0; i < 8; i++) {
        (byte & 0x80) ? SDA_HIGH() : SDA_LOW();
        byte <<= 1;
        I2C_DELAY();
        SCL_HIGH();
        I2C_DELAY();
        SCL_LOW();
    }
    
    // 等待ACK
    SDA_HIGH();
    SCL_HIGH();
    I2C_DELAY();
    uint8_t ack = !SDA_READ();
    SCL_LOW();
    if (ack == 0) {
			printf("write ack err\r\n");
    }
    return ack;
}

static uint8_t i2c_read_byte(uint8_t ack) {
    uint8_t byte = 0;
    SDA_HIGH();
    
    for(uint8_t i = 0; i < 8; i++) {
        byte <<= 1;
        SCL_HIGH();
        I2C_DELAY();
        if(SDA_READ()) byte |= 0x01;
        SCL_LOW();
        I2C_DELAY();
    }

    // 发送ACK/NACK
    ack ? SDA_LOW() : SDA_HIGH();
    SCL_HIGH();
    I2C_DELAY();
    SCL_LOW();
    SDA_HIGH();
    if (ack == 0 && SDA_READ() == GPIO_PIN_SET) { 
			printf("read ack err\r\n");
    }
    return byte;
}

/* 修改原有I2C通信函数 */
void DS1307_SetRegByte(uint8_t regAddr, uint8_t val) {
    i2c_start();
    i2c_write_byte(DS1307_I2C_ADDR << 1); // 写模式
    i2c_write_byte(regAddr);
    i2c_write_byte(val);
    i2c_stop();
}

uint8_t DS1307_GetRegByte(uint8_t regAddr) {
    uint8_t val;
    i2c_start();
    i2c_write_byte(DS1307_I2C_ADDR << 1); // 写模式发送寄存器地址
    i2c_write_byte(regAddr);
    i2c_start();                          // 重复起始条件
    i2c_write_byte((DS1307_I2C_ADDR << 1) | 0x01); // 读模式
    val = i2c_read_byte(0);               // 最后读取NACK
    i2c_stop();
    return val;
}

/* 修改初始化函数 */
void DS1307_Init(void) { 
    SCL_HIGH();
    SDA_HIGH();
    
    DS1307_SetClockHalt(0);
}
void DS1307_SetClockHalt(uint8_t halt) {
	uint8_t ch = (halt ? 1 << 7 : 0);
	DS1307_SetRegByte(DS1307_REG_SECOND, ch | (DS1307_GetRegByte(DS1307_REG_SECOND) & 0x7f));
}
uint8_t DS1307_GetClockHalt(void) {
	return (DS1307_GetRegByte(DS1307_REG_SECOND) & 0x80) >> 7;
}


/**
 * @brief Toggle square wave output on pin 7.
 * @param mode DS1307_ENABLED (1) or DS1307_DISABLED (0);
 */
void DS1307_SetEnableSquareWave(DS1307_SquareWaveEnable mode){
	uint8_t controlReg = DS1307_GetRegByte(DS1307_REG_CONTROL);
	uint8_t newControlReg = (controlReg & ~(1 << 4)) | ((mode & 1) << 4);
	DS1307_SetRegByte(DS1307_REG_CONTROL, newControlReg);
}

/**
 * @brief Set square wave output frequency.
 * @param rate DS1307_1HZ (0b00), DS1307_4096HZ (0b01), DS1307_8192HZ (0b10) or DS1307_32768HZ (0b11).
 */
void DS1307_SetInterruptRate(DS1307_Rate rate){
	uint8_t controlReg = DS1307_GetRegByte(DS1307_REG_CONTROL);
	uint8_t newControlReg = (controlReg & ~0x03) | rate;
	DS1307_SetRegByte(DS1307_REG_CONTROL, newControlReg);
}

/**
 * @brief Gets the current day of week.
 * @return Days from last Sunday, 0 to 6.
 */
uint8_t DS1307_GetDayOfWeek(void) {
	return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_DOW));
}

/**
 * @brief Gets the current day of month.
 * @return Day of month, 1 to 31.
 */
uint8_t DS1307_GetDate(void) {
	return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_DATE));
}

/**
 * @brief Gets the current month.
 * @return Month, 1 to 12.
 */
uint8_t DS1307_GetMonth(void) {
	return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MONTH));
}

/**
 * @brief Gets the current year.
 * @return Year, 2000 to 2099.
 */
uint16_t DS1307_GetYear(void) {
	uint16_t cen = DS1307_GetRegByte(DS1307_REG_CENT) * 100;
	return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_YEAR)) + cen;
}

/**
 * @brief Gets the current hour in 24h format.
 * @return Hour in 24h format, 0 to 23.
 */
uint8_t DS1307_GetHour(void) {
	return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_HOUR) & 0x3f);
}

/**
 * @brief Gets the current minute.
 * @return Minute, 0 to 59.
 */
uint8_t DS1307_GetMinute(void) {
	return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_MINUTE));
}

/**
 * @brief Gets the current second. Clock halt bit not included.
 * @return Second, 0 to 59.
 */
uint8_t DS1307_GetSecond(void) {
	return DS1307_DecodeBCD(DS1307_GetRegByte(DS1307_REG_SECOND) & 0x7f);
}

/**
 * @brief Gets the stored UTC hour offset.
 * @note  UTC offset is not updated automatically.
 * @return UTC hour offset, -12 to 12.
 */
int8_t DS1307_GetTimeZoneHour(void) {
	return DS1307_GetRegByte(DS1307_REG_UTC_HR);
}

/**
 * @brief Gets the stored UTC minute offset.
 * @note  UTC offset is not updated automatically.
 * @return UTC time zone, 0 to 59.
 */
uint8_t DS1307_GetTimeZoneMin(void) {
	return DS1307_GetRegByte(DS1307_REG_UTC_MIN);
}

/**
 * @brief Sets the current day of week.
 * @param dayOfWeek Days since last Sunday, 0 to 6.
 */
void DS1307_SetDayOfWeek(uint8_t dayOfWeek) {
	DS1307_SetRegByte(DS1307_REG_DOW, DS1307_EncodeBCD(dayOfWeek));
}

/**
 * @brief Sets the current day of month.
 * @param date Day of month, 1 to 31.
 */
void DS1307_SetDate(uint8_t date) {
	DS1307_SetRegByte(DS1307_REG_DATE, DS1307_EncodeBCD(date));
}

/**
 * @brief Sets the current month.
 * @param month Month, 1 to 12.
 */
void DS1307_SetMonth(uint8_t month) {
	DS1307_SetRegByte(DS1307_REG_MONTH, DS1307_EncodeBCD(month));
}

/**
 * @brief Sets the current year.
 * @param year Year, 2000 to 2099.
 */
void DS1307_SetYear(uint16_t year) {
	DS1307_SetRegByte(DS1307_REG_CENT, year / 100);
	DS1307_SetRegByte(DS1307_REG_YEAR, DS1307_EncodeBCD(year % 100));
}

/**
 * @brief Sets the current hour, in 24h format.
 * @param hour_24mode Hour in 24h format, 0 to 23.
 */
void DS1307_SetHour(uint8_t hour_24mode) {
	DS1307_SetRegByte(DS1307_REG_HOUR, DS1307_EncodeBCD(hour_24mode & 0x3f));
}

/**
 * @brief Sets the current minute.
 * @param minute Minute, 0 to 59.
 */
void DS1307_SetMinute(uint8_t minute) {
	DS1307_SetRegByte(DS1307_REG_MINUTE, DS1307_EncodeBCD(minute));
}

/**
 * @brief Sets the current second.
 * @param second Second, 0 to 59.
 */
void DS1307_SetSecond(uint8_t second) {
	uint8_t ch = DS1307_GetClockHalt();
	DS1307_SetRegByte(DS1307_REG_SECOND, DS1307_EncodeBCD(second | ch));
}

/**
 * @brief Sets UTC offset.
 * @note  UTC offset is not updated automatically.
 * @param hr UTC hour offset, -12 to 12.
 * @param min UTC minute offset, 0 to 59.
 */
void DS1307_SetTimeZone(int8_t hr, uint8_t min) {
	DS1307_SetRegByte(DS1307_REG_UTC_HR, hr);
	DS1307_SetRegByte(DS1307_REG_UTC_MIN, min);
}

/**
 * @brief Decodes the raw binary value stored in registers to decimal format.
 * @param bin Binary-coded decimal value retrieved from register, 0 to 255.
 * @return Decoded decimal value.
 */
uint8_t DS1307_DecodeBCD(uint8_t bin) {
    if (((bin & 0xF0) >> 4) > 9 || (bin & 0x0F) > 9) {
        return 0;
    }
    return (((bin & 0xF0) >> 4) * 10) + (bin & 0x0F);
}

/**
 * @brief Encodes a decimal number to binaty-coded decimal for storage in registers.
 * @param dec Decimal number to encode.
 * @return Encoded binary-coded decimal value.
 */
uint8_t DS1307_EncodeBCD(uint8_t dec) {
    if (dec > 99) {
        return 0;
    }
    return (dec % 10) + ((dec / 10) << 4);
}
