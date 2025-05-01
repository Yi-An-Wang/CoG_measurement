/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DT_Pin1 	GPIO_PIN_10
#define DT_Pin2 	GPIO_PIN_8
#define DT_Pin3 	GPIO_PIN_4
#define DT_Pin4 	GPIO_PIN_6

#define SDK_Pin1 	GPIO_PIN_11
#define SDK_Pin2 	GPIO_PIN_9
#define SDK_Pin3 	GPIO_PIN_5
#define SDK_Pin4 	GPIO_PIN_7

#define DT_Port 	GPIOA
#define SDK_Port	GPIOA

#define ADXL345_ADDR       (0x53 << 1)  // 注意：I2C位址左移1位（因為HAL使用8位元格式）
#define ADXL345_DEVID_REG  0x00
#define ADXL345_POWER_CTL  0x2D
#define ADXL345_DATAX0     0x32

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// ----------------- hx711 begin ---------------------------
int32_t load_num1, load_num2, load_num3, load_num4;
int64_t load_sum1, load_sum2, load_sum3, load_sum4;
int32_t avg_load1, avg_load2, avg_load3, avg_load4;

// parameters for calibration
int32_t tare1 = 8858327, tare2 = 8854343, tare3 = 8783827, tare4 = 8231851;
float weight_c = 5 * 1000000; //in micrograme
int32_t weight_load1=332172, weight_load2=177465, weight_load3=266040, weight_load4=810000;
float force1, force2, force3, force4;

// defined parameters for testing and debug
int c_in_count = 0;
int down = 0;
// ------------------ hx711 end ----------------------------

// ------------------ ADXL345 begin ------------------------
int16_t gx_num, gy_num, gz_num;
float ax, ay, az;
// ------------------ ADXL345 end --------------------------

// ------------------ send to serial -----------------------
char msg[64];
// ------------------ send to serial end -------------------

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HX711_delay_us(uint32_t us)
{
    // manual timer delay
    for (volatile uint32_t i = 0; i < us * 8; i++);
}

int32_t HX711_Read(GPIO_TypeDef* HX711_DT_GPIO_Port, uint16_t HX711_DT_Pin, GPIO_TypeDef* HX711_SCK_GPIO_Port, uint16_t HX711_SCK_Pin)
{
    int32_t count = 0;
    uint8_t i;

    down = 0;
    // waiting for DOUT down, data readable
    while(HAL_GPIO_ReadPin(HX711_DT_GPIO_Port, HX711_DT_Pin));
    down = 1;

    for (i = 0; i < 24; i++)
    {
        HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_SET);
        HX711_delay_us(1);

        count = count << 1;
        if (HAL_GPIO_ReadPin(HX711_DT_GPIO_Port, HX711_DT_Pin))
            count++;

        HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);
        HX711_delay_us(1);
    }

    // 第25個時脈：設定增益 (這裡預設 128)
    HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_SET);
    HX711_delay_us(1);
    HAL_GPIO_WritePin(HX711_SCK_GPIO_Port, HX711_SCK_Pin, GPIO_PIN_RESET);
    HX711_delay_us(1);

    // 轉成補數
    count ^= 0x800000;
    return count;
}

void ADXL345_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t data;

    // 確認 device ID 是否正確（應為 0xE5）
    HAL_I2C_Mem_Read(hi2c, ADXL345_ADDR, ADXL345_DEVID_REG, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    if (data != 0xE5) {
        // 錯誤處理
    }

    // 開啟測量模式（將 POWER_CTL 的 bit3 設為 1）
    data = 0x08;
    HAL_I2C_Mem_Write(hi2c, ADXL345_ADDR, ADXL345_POWER_CTL, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

void ADXL345_ReadXYZ(I2C_HandleTypeDef *hi2c, int16_t *x, int16_t *y, int16_t *z) {
    uint8_t buffer[6];
    HAL_I2C_Mem_Read(hi2c, ADXL345_ADDR, ADXL345_DATAX0, I2C_MEMADD_SIZE_8BIT, buffer, 6, HAL_MAX_DELAY);
    *x = (int16_t)(buffer[1] << 8 | buffer[0]);
    *y = (int16_t)(buffer[3] << 8 | buffer[2]);
    *z = (int16_t)(buffer[5] << 8 | buffer[4]);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  int count_num = 0;
  load_sum1 = 0;
  load_sum2 = 0;
  load_sum3 = 0;
  load_sum4 = 0;

  ADXL345_Init(&hi2c1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	c_in_count++;

	load_num1 = HX711_Read(DT_Port, DT_Pin1, SDK_Port, SDK_Pin1);
	load_num2 = HX711_Read(DT_Port, DT_Pin2, SDK_Port, SDK_Pin2);
	load_num3 = HX711_Read(DT_Port, DT_Pin3, SDK_Port, SDK_Pin3);
	load_num4 = HX711_Read(DT_Port, DT_Pin4, SDK_Port, SDK_Pin4);

	weight_load1 = load_num1 - tare1;
	weight_load2 = load_num2 - tare2;
	weight_load3 = load_num3 - tare3;
	weight_load4 = load_num4 = tare4;

	force1 = (load_num1 - tare1) * (weight_load1/(weight_c/4)) * 0.001;
	force2 = (load_num2 - tare2) * (weight_load2/(weight_c/4)) * 0.001;
	force3 = (load_num3 - tare3) * (weight_load3/(weight_c/4)) * 0.001;
	force4 = (load_num4 - tare4) * (weight_load4/(weight_c/4)) * 0.001;

	if (count_num < 500){
		load_sum1 = load_sum1 + load_num1;
		load_sum2 = load_sum2 + load_num2;
		load_sum3 = load_sum3 + load_num3;
		load_sum4 = load_sum4 + load_num4;
		count_num++;
	}
	else{
		avg_load1 = load_sum1 / count_num;
		avg_load2 = load_sum2 / count_num;
		avg_load3 = load_sum3 / count_num;
		avg_load4 = load_sum4 / count_num;

		count_num = 0;
		load_sum1 = 0;
		load_sum2 = 0;
		load_sum3 = 0;
		load_sum4 = 0;
	}

	ADXL345_ReadXYZ(&hi2c1, &gx_num, &gy_num, &gz_num);
	ax = gx_num / 256.0;
	ay = gy_num / 256.0;
	az = gy_num / 256.0;

	int len = sprintf(msg, "%.6f,%.6f,%.6f,%.6f,&.6f,%.6f,%.6f\r\n", force1, force2, force3, force4, ax, ay, az);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 1);

	HAL_Delay(50);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 15;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
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
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA6 PA8 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA7 PA9 PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
