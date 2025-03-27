/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "w25qxx.h"
#include "ATcmd.h"
#include "bl.h"

SemaphoreHandle_t xSemaphoreUSART1;
TaskHandle_t xReceiveTaskHandle;

SemaphoreHandle_t xSemaphoreUSART3;
TaskHandle_t xBTReceiveTaskHandle;

// TaskHandle_t FlashWriteDateTaskHandle;

#define USART1_RX_BUF_SIZE 512
#define USART1_TX_BUF_SIZE 512
uint8_t USART1_RX_Buff[USART1_RX_BUF_SIZE] = {0};
uint8_t USART1_TX_Buff[USART1_TX_BUF_SIZE] = {0};
volatile uint8_t USART1_RC_Flag = 0;
volatile uint8_t USART1_TC_Flag = 0;

volatile uint16_t received_len = 0;

#define USART3_RX_BUF_SIZE 4096
#define USART3_TX_BUF_SIZE 512
uint8_t USART3_RX_Buff[USART3_RX_BUF_SIZE] = {0};
uint8_t USART3_TX_Buff[USART3_TX_BUF_SIZE] = {0};
volatile uint8_t USART3_RC_Flag = 0;
volatile uint8_t USART3_TC_Flag = 0;

volatile uint16_t received_len3 = 0;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */
  xSemaphoreUSART1 = xSemaphoreCreateMutex();
  xTaskCreate(vReceiveTask, "ReceiveTask", 256, NULL, 6, &xReceiveTaskHandle);
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  HAL_UART_Receive_DMA(&huart1, (uint8_t *)USART1_RX_Buff, USART1_RX_BUF_SIZE);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */
  xSemaphoreUSART3 = xSemaphoreCreateMutex();
  xTaskCreate(vBTReceiveTask, "BTReceiveTask", 256, NULL, 6, &xBTReceiveTaskHandle);
  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
  HAL_UART_Receive_DMA(&huart3, (uint8_t *)USART3_RX_Buff, USART3_RX_BUF_SIZE);
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_2;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_2;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA1_Channel3;
    hdma_usart3_rx.Init.Request = DMA_REQUEST_2;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);

    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA1_Channel2;
    hdma_usart3_tx.Init.Request = DMA_REQUEST_2;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart3_tx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int fputc(int ch, FILE *fp)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

int fgetc(FILE *fp)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}

void USART1_DMA_Send(uint8_t *data, uint32_t length)
{
  if (length > USART1_TX_BUF_SIZE)
    return;
  if (xSemaphoreTake(xSemaphoreUSART1, portMAX_DELAY) == pdTRUE)
  {
    if (HAL_UART_Transmit_DMA(&huart1, data, length) != HAL_OK)
    {
      printf("USART1 DMA Send Error!\r\n");
    }
    xSemaphoreGive(xSemaphoreUSART1);
  }
}

void USART3_DMA_Send(uint8_t *data, uint32_t length)
{
  if (length > USART3_TX_BUF_SIZE)
    return;
  if (xSemaphoreTake(xSemaphoreUSART3, portMAX_DELAY) == pdTRUE)
  {
    if (HAL_UART_Transmit_DMA(&huart3, data, length) != HAL_OK)
    {
      printf("USART3 DMA Send Error!\r\n");
    }
    xSemaphoreGive(xSemaphoreUSART3);
  }
}

void USART1_DMA_Receive(uint8_t *data, uint32_t *length)
{
  if (xSemaphoreTake(xSemaphoreUSART1, portMAX_DELAY) == pdTRUE)
  {
    if (USART1_RC_Flag)
    {
      USART1_RC_Flag = 0;
      memcpy(data, USART1_RX_Buff, received_len);
      *length = received_len;
      received_len = 0;
    }
    xSemaphoreGive(xSemaphoreUSART1);
  }
}

void USART3_DMA_Receive(uint8_t *data, uint32_t *length)
{
  if (xSemaphoreTake(xSemaphoreUSART3, portMAX_DELAY) == pdTRUE)
  {
    if (USART3_RC_Flag)
    {
      USART3_RC_Flag = 0;
      memcpy(data, USART3_RX_Buff, received_len3);
      *length = received_len3;
      received_len3 = 0;
    }
    xSemaphoreGive(xSemaphoreUSART3);
  }
}

void USART1_DMA_StartReceive(void)
{
  if (HAL_UART_Receive_DMA(&huart1, USART1_RX_Buff, USART1_RX_BUF_SIZE) != HAL_OK)
  {
    printf("USART1 DMA Receive Error!\r\n");
  }
}

void USART3_DMA_StartReceive(void)
{
  if (HAL_UART_Receive_DMA(&huart3, USART3_RX_Buff, USART3_RX_BUF_SIZE) != HAL_OK)
  {
    printf("USART3 DMA Receive Error!\r\n");
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    USART1_TC_Flag = 0;
  }
  if (huart->Instance == USART3)
  {
    USART3_TC_Flag = 0;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    USART1_RC_Flag = 1;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(xReceiveTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
  if (huart->Instance == USART3)
  {
    USART3_RC_Flag = 1;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyFromISR(xBTReceiveTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void USAR_USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
  if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    HAL_UART_DMAStop(&huart1);

    received_len = USART1_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    if (received_len > 0 && received_len <= USART1_RX_BUF_SIZE)
    {

      USART1_RC_Flag = 1;
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      xTaskNotifyFromISR(xReceiveTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);

      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    USART1_DMA_StartReceive();
  }
}

void USAR_USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart3);
  if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(&huart3);
    HAL_UART_DMAStop(&huart3);

    received_len3 = USART3_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart3.hdmarx);
    if (received_len3 > 0 && received_len3 <= USART3_RX_BUF_SIZE)
    {

      USART3_RC_Flag = 1;
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//      xTaskNotifyFromISR(xBTReceiveTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
			xTaskNotifyFromISR(xReceiveTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    USART3_DMA_StartReceive();
  }
}

uint8_t rx_data[USART1_RX_BUF_SIZE];
uint32_t rx_length = 0;
void vReceiveTask(void *pvParameters)
{
  static uint32_t total_written = 0;
  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (USART1_RC_Flag)
    {
      USART1_DMA_Receive(rx_data, &rx_length);
      //					USART1_DMA_Send(rx_data,rx_length);
      if (AT_FlashWrite_Flag == 0)
      {
        AT_Command_Parser((char *)rx_data);
      }
      else if (AT_FlashWrite_Flag == 1)
      {
        if (total_written < at_write_size)
        {
          //							W25QXX_Write(rx_data, at_write_addr + total_written, rx_length);
          Flash_Write(rx_data, at_write_addr + total_written, rx_length);
          total_written += rx_length;
          printf("Written %d bytes, Total: %d/%d\n", rx_length, total_written, at_write_size);
          if (total_written >= at_write_size)
          {
            AT_FlashWrite_Flag = 0;
            at_write_addr = 0;
            at_write_size = 0;
            total_written = 0;
						HAL_FLASH_Lock();
            printf("Write complete.\n");
          }
        }
      }
    }
   else if (USART3_RC_Flag)
    {
      USART3_DMA_Receive(rx_data, &rx_length);
      //					USART1_DMA_Send(rx_data,rx_length);
      if (AT_FlashWrite_Flag == 0)
      {
        AT_Command_Parser((char *)rx_data);
      }
      else if (AT_FlashWrite_Flag == 1)
      {
        if (total_written < at_write_size)
        {
          //							W25QXX_Write(rx_data, at_write_addr + total_written, rx_length);
          Flash_Write(rx_data, at_write_addr + total_written, rx_length);
          total_written += rx_length;
          printf("Written %d bytes, Total: %d/%d\n", rx_length, total_written, at_write_size);
          if (total_written >= at_write_size)
          {
            AT_FlashWrite_Flag = 0;
            at_write_addr = 0;
            at_write_size = 0;
            total_written = 0;
						HAL_FLASH_Lock();
            printf("Write complete.\n");
          }
        }
      }
    }
  }
}

uint8_t bt_rx_data[USART3_RX_BUF_SIZE];
uint32_t bt_rx_length = 0;
void vBTReceiveTask(void *pvParameters)
{
  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (USART3_RC_Flag)
    {
      USART3_DMA_Receive(bt_rx_data, &bt_rx_length);
      USART1_DMA_Send(bt_rx_data, bt_rx_length);
//			USART3_DMA_Send(bt_rx_data, bt_rx_length);
    }
  }
}

/* USER CODE END 1 */
