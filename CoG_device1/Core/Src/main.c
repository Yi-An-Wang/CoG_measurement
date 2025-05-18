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

#include "mma845x.h"
#include <math.h>
#include <stdio.h>

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

#define DT1  GPIO_PIN_0
#define DT2  GPIO_PIN_2
#define DT3  GPIO_PIN_4
#define DT4  GPIO_PIN_6
#define DT_Port GPIOC

#define SCK1  GPIO_PIN_1
#define SCK2  GPIO_PIN_3
#define SCK3  GPIO_PIN_5
#define SCK4  GPIO_PIN_7
#define SCK_Port GPIOC

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// ------------------ variables for hx711 --------------------------
uint32_t raw1;
uint32_t raw2;
uint32_t raw3;
uint32_t raw4;

int count_num = 0;

float tare1 = 8298487.5;
float tare2 = 8670518;
float tare3 = 8617560;
float tare4 = 9115431;

float new_weight1;
float new_weight2;
float new_weight3;
float new_weight4;

float estimated_weight1 = 0.0;
float estimated_weight2 = 0.0;
float estimated_weight3 = 0.0;
float estimated_weight4 = 0.0;
// ------------------ variables for hx711 end --------------------------

// ------------------ variables for MMA8451 ----------------------------
//uint8_t who_am_i;
int16_t ax, ay, az;
float est_ax = 0, est_ay = 0, est_az = 1000;

float raw_pitch, raw_roll;
float pitch, roll;
// ------------------ variables for MMA8451 end ------------------------

// ------------------ variables for button -----------------------------
uint8_t reset;
uint8_t record;
uint8_t break_python;
// ------------------ variables for button end -------------------------

// ------------------ variables for output -----------------------------

char msg[256];

// ------------------ variables for output end -------------------------

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HX711_delay_us(uint32_t us)
{
    // 需要微秒級延遲，可以用 Timer 或簡單迴圈微調
    for (volatile uint32_t i = 0; i < us * 8; i++);
}

int32_t HX711_Read(GPIO_TypeDef* HX711_DT_GPIO_Port, uint16_t HX711_DT_Pin, GPIO_TypeDef* HX711_SCK_GPIO_Port, uint16_t HX711_SCK_Pin)
{
    int32_t count = 0;
    uint8_t i;

    // 等待 DOUT 變低，表示資料可讀
    while(HAL_GPIO_ReadPin(HX711_DT_GPIO_Port, HX711_DT_Pin));

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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  int count_valve = 200;

//  float know_kg1 = 2.5;
//  float know_read1 = 1072407;
//  float know_kg2 = 2.5;
//  float know_read2 = 911823.75;
//  float know_kg3 = 2.5;
//  float know_read3 = 964234.062;
//  float know_kg4 = 2.5;
//  float know_read4 = 1111481.38;

  float know_kg1 = 2.5;
  float know_read1 = 1125744.38;
  float know_kg2 = 2.5;
  float know_read2 = 878821.812;
  float know_kg3 = 2.5;
  float know_read3 = 993117.375;
  float know_kg4 = 2.5;
  float know_read4 = 1069001.75;

//  float know_kg1 = 1;
//  float know_read1 = 1;
//  float know_kg2 = 1;
//  float know_read2 = 1;
//  float know_kg3 = 1;
//  float know_read3 = 1;
//  float know_kg4 = 1;
//  float know_read4 = 1;

  float coefficient1 = know_kg1 / know_read1;
  float coefficient2 = know_kg2 / know_read2;
  float coefficient3 = know_kg3 / know_read3;
  float coefficient4 = know_kg4 / know_read4;

  float alpha = 0.9;

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  MMA845x_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	// ------------------- HX711_part ----------------------------
	raw1 = HX711_Read(DT_Port, DT1, SCK_Port, SCK1);
	raw2 = HX711_Read(DT_Port, DT2, SCK_Port, SCK2);
	raw3 = HX711_Read(DT_Port, DT3, SCK_Port, SCK3);
	raw4 = HX711_Read(DT_Port, DT4, SCK_Port, SCK4);

	if (count_num <= count_valve) {
		tare1 = tare1 * alpha + raw1 * (1-alpha);
		tare2 = tare2 * alpha + raw2 * (1-alpha);
		tare3 = tare3 * alpha + raw3 * (1-alpha);
		tare4 = tare4 * alpha + raw4 * (1-alpha);
		count_num++;
	}
	else {
		new_weight1 = (raw1 - tare1) * coefficient1;
		new_weight2 = (raw2 - tare2) * coefficient2;
		new_weight3 = (raw3 - tare3) * coefficient3;
		new_weight4 = (raw4 - tare4) * coefficient4;
		estimated_weight1 = alpha * estimated_weight1 + (1-alpha) * new_weight1;
		estimated_weight2 = alpha * estimated_weight2 + (1-alpha) * new_weight2;
		estimated_weight3 = alpha * estimated_weight3 + (1-alpha) * new_weight3;
		estimated_weight4 = alpha * estimated_weight4 + (1-alpha) * new_weight4;
	}
	// ------------------- HX711_part End ----------------------------

	// ------------------- MMA8451_part ------------------------------

//	HAL_I2C_Mem_Read(&hi2c1, MMA845X_ADDR, 0x0D, 1, &who_am_i, 1, HAL_MAX_DELAY);
	MMA845x_ReadXYZ(&ax, &ay, &az);
	est_ax = alpha * est_ax + (1-alpha) * ax;
	est_ay = alpha * est_ay + (1-alpha) * ay;
	est_az = alpha * est_az + (1-alpha) * az;

	raw_pitch = atan2(-ax, sqrt(ay*ay + az*az));
	raw_roll = atan2(ay, az);

	pitch = atan2(-est_ax, sqrt(est_ay*est_ay + est_az*est_az));
	roll = atan2(est_ay, est_az);
	// ------------------- MMA8451_part End --------------------------

	// ------------------- button detect -----------------------------
	reset = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	record = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	break_python = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);

	if (reset > 0) {
		count_num = 0;
		tare1 = 8298487.5;
		tare2 = 8670518;
		tare3 = 8617560;
		tare4 = 9115431;
		new_weight1 = 0.0;
		new_weight2 = 0.0;
		new_weight3 = 0.0;
		new_weight4 = 0.0;
		estimated_weight1 = 0.0;
		estimated_weight2 = 0.0;
		estimated_weight3 = 0.0;
		estimated_weight4 = 0.0;
		HAL_Delay(1000);
	}

	int len = sprintf(msg, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\r\n",
			new_weight1, new_weight2, new_weight3, new_weight4, estimated_weight1, estimated_weight2, estimated_weight3, estimated_weight4,
			raw_pitch, raw_roll, pitch, roll, reset, record, break_python);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, HAL_MAX_DELAY);

	HAL_Delay(200);
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
  hi2c1.Init.ClockSpeed = 400000;
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
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC2 PC4 PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC1 PC3 PC5 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
