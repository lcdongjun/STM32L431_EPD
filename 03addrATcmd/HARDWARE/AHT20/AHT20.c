#include "aht20.h"

// 定义软件I2C引脚
#define AHT20_SCL_PIN         GPIO_PIN_6
#define AHT20_SCL_PORT        GPIOB
#define AHT20_SDA_PIN         GPIO_PIN_7
#define AHT20_SDA_PORT        GPIOB

// 时序延迟宏（根据实际时钟速度调整）
#define I2C_DELAY()     Delay_us(100)

// 私有函数声明
static void I2C_Start(void);
static void I2C_Stop(void);
static bool I2C_WriteByte(uint8_t byte);
static bool I2C_ReadByte(uint8_t *byte, bool ack);
static bool AHT20_WaitForReady(void);
static bool AHT20_SoftReset(void);

bool AHT20_Init(void) {
    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    GPIO_InitStruct.Pin = AHT20_SCL_PIN;
    HAL_GPIO_Init(AHT20_SCL_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = AHT20_SDA_PIN;
    HAL_GPIO_Init(AHT20_SDA_PORT, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_SET);

    // 软件复位
    if(!AHT20_SoftReset()) return false;
		vTaskDelay(pdMS_TO_TICKS(20));

    // 发送初始化命令
    I2C_Start();
    if(!I2C_WriteByte(AHT20_I2C_ADDRESS << 1)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(0xBE)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(0x08)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(0x00)) { I2C_Stop(); return false; }
    I2C_Stop();

		vTaskDelay(pdMS_TO_TICKS(10));
    return AHT20_WaitForReady();
}

bool AHT20_ReadData(AHT20_Data *data) {
    uint8_t buffer[6] = {0};
    
    // 触发测量
    I2C_Start();
    if(!I2C_WriteByte(AHT20_I2C_ADDRESS << 1)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(0xAC)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(0x33)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(0x00)) { I2C_Stop(); return false; }
    I2C_Stop();

    // 等待测量完成
    if(!AHT20_WaitForReady()) return false;

    // 读取数据
    I2C_Start();
    if(!I2C_WriteByte((AHT20_I2C_ADDRESS << 1) | 0x01)) { I2C_Stop(); return false; }
    
    for(uint8_t i = 0; i < 6; i++) {
        if(!I2C_ReadByte(&buffer[i], (i < 5))) {
            I2C_Stop();
            return false;
        }
    }
    I2C_Stop();

    // 数据转换
    uint32_t hum_raw = ((uint32_t)buffer[1] << 12) | ((uint32_t)buffer[2] << 4) | (buffer[3] >> 4);
    uint32_t temp_raw = ((uint32_t)(buffer[3] & 0x0F) << 16) | ((uint32_t)buffer[4] << 8) | buffer[5];

    data->humidity = (float)hum_raw / (1UL << 20) * 100.0f;
    data->temperature = (float)temp_raw / (1UL << 20) * 200.0f - 50.0f;

    return true;
}

static bool AHT20_WaitForReady(void) {
    uint32_t timeout = 1000;
    uint8_t status;
    
    do {
        I2C_Start();
        if(!I2C_WriteByte((AHT20_I2C_ADDRESS << 1) | 0x01)) {
            I2C_Stop();
            return false;
        }
        if(!I2C_ReadByte(&status, false)) {
            I2C_Stop();
            return false;
        }
        I2C_Stop();
        
        if((status & 0x80) == 0x00) return true;
				vTaskDelay(pdMS_TO_TICKS(2));
    } while(timeout-- > 0);
    
    return false;
}

static bool AHT20_SoftReset(void) {
    I2C_Start();
    if(!I2C_WriteByte(AHT20_I2C_ADDRESS << 1)) { I2C_Stop(); return false; }
    if(!I2C_WriteByte(0xBA)) { I2C_Stop(); return false; }
    I2C_Stop();
		vTaskDelay(pdMS_TO_TICKS(20));
    return true;
}

// 软件I2C基础函数
static void I2C_Start(void) {
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_RESET);
    I2C_DELAY();
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_RESET);
    I2C_DELAY();
}

static void I2C_Stop(void) {
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_SET);
    I2C_DELAY();
}

static bool I2C_WriteByte(uint8_t byte) {
    for(uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, (byte & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        byte <<= 1;
        HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_SET);
        I2C_DELAY();
        HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_RESET);
        I2C_DELAY();
    }

    // 检查ACK
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    bool ack = (HAL_GPIO_ReadPin(AHT20_SDA_PORT, AHT20_SDA_PIN) == GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_RESET);
    I2C_DELAY();
    return ack;
}

static bool I2C_ReadByte(uint8_t *byte, bool ack) {
    uint8_t data = 0;
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_SET);
    
    for(uint8_t i = 0; i < 8; i++) {
        data <<= 1;
        HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_SET);
        I2C_DELAY();
        if(HAL_GPIO_ReadPin(AHT20_SDA_PORT, AHT20_SDA_PIN)) data |= 0x01;
        HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_RESET);
        I2C_DELAY();
    }

    // 发送ACK/NACK
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, ack ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_SET);
    I2C_DELAY();
    HAL_GPIO_WritePin(AHT20_SCL_PORT, AHT20_SCL_PIN, GPIO_PIN_RESET);
    I2C_DELAY();
    HAL_GPIO_WritePin(AHT20_SDA_PORT, AHT20_SDA_PIN, GPIO_PIN_SET);
    
    *byte = data;
    return true;
}

