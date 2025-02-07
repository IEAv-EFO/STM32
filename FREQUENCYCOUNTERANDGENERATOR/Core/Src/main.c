/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "count2volt.h"
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ARRMAX 65535
//#define FREQGENON
#define FREQGENOFF
#define LCD
#define USB
#define BT
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
HAL_StatusTypeDef retTimer;
GPIO_PinState pinState;
char bufferCDC[1000], bufferBT[1000], bufferLCD[50];
char bufferGenFreq[20], bufferFreq[20], bufferTimerClock[20];
uint8_t length, flag = 0, flagBT = 1;
uint32_t period, desiredFreq, periodPrint, PSC, ARR, CCR, counter = 0;
uint32_t timerClock, desiredFreqperiod, currentCount, lastCountPrint, lastCount = 0;
float dCycle, genFreq, frequency, freqPrint;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void freqGen(TIM_HandleTypeDef *htim, uint32_t freq);
void freqConv(float freq, char Buf[], uint8_t dPlaces);
int countDigits(int number);
void print2LCD();
void print2USB();
void print2BT();
void btSendString(char *Buf);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void lcd_init(void);   // initialize lcd
void lcd_send_cmd(char cmd);  // send command to the lcd
void lcd_send_data(char data);  // send data to the lcd
void lcd_send_string(char *str);  // send string to the lcd
void lcd_put_cur(int row, int col); // put cursor at the entered position row (0 or 1), col (0-15);
void lcd_clear(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	timerClock = (float) HAL_RCC_GetPCLK2Freq();

	#ifdef FREQGENON
		desiredFreq = 12000;
	#endif

	lcd_init();
	HAL_Delay(100);
	lcd_clear();
	HAL_Delay(100);

	retTimer = HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	if (retTimer == HAL_OK) {
		HAL_Delay(200);
		for (uint8_t i = 0; i < 6; i++) {
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			HAL_Delay(200);
		}
	}

#ifdef FREQGENON
		freqGen(&htim3, desiredFreq);
		freqConv(genFreq, bufferGenFreq, 4);
	#endif

	freqConv(timerClock, bufferTimerClock, 2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		freqConv(frequency, bufferFreq, 4);

		dCycle = (CCR / (float) (ARR + 1)) * 100.0;

		length = countDigits(period);

		#ifdef LCD && #undef USB && #undef BT
			print2LCD();
		#endif

		#ifdef USB && #undef BT && #undef LCD
			//BufferCDC com CRLF para a porta USB
			print2USB();
		#endif

		#ifdef BT && #undef USB && #undef LCD
			//BufferBT com LF para o bluetooth
			print2BT();
		#endif

		if (flag) {
			TIM2->CNT = 0;
			flag = 0;
		}

		HAL_Delay(1000);

		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void freqGen(TIM_HandleTypeDef *htim, uint32_t freq) {
	if (htim->Instance == TIM3) {
		uint32_t arr, psc = 1;
		while (1) {
			if (timerClock % freq == 0) {
				arr = (timerClock / (freq * psc)) - 1;
				if (arr <= ARRMAX) {
					break;
				}
				psc++;
			}
		}
		psc -= 1;

		genFreq = (float) freq;
		ARR = arr;
		CCR = (ARR + 1) / 2;
		PSC = psc;

		htim->Instance->ARR = ARR;
		htim->Instance->PSC = psc;
		htim->Instance->CCR1 = CCR;

		HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
	}
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}

void freqConv(float freq, char Buf[], uint8_t dPlaces) {
	uint16_t oMagnitude = 0;
	while (freq >= 1000) {
		freq /= 1000.0;
		oMagnitude++;
	}

	switch (oMagnitude) {
	case 0:
		sprintf(Buf, "%.*f [Hz]", dPlaces, freq);
		break;
	case 1:
		sprintf(Buf, "%.*f [KHz]", dPlaces, freq);
		break;
	case 2:
		sprintf(Buf, "%.*f [MHz]", dPlaces, freq);
		break;
	case 3:
		sprintf(Buf, "%.*f [GHz]", dPlaces, freq);
		break;
	default:
		sprintf(Buf, "Out of range");
		break;
	}
}

int countDigits(int number) {
	if (number == 0) {
		return 1;
	}

	int count = 0;

	// Handle negative numbers
	if (number < 0) {
		number = -number;
	}

	while (number != 0) {
		number /= 10;
		count++;
	}

	return count;
}

void print2LCD() {
	lcd_put_cur(0, 0);
	sprintf(bufferLCD, "%s", "Freq = ");
	lcd_send_string(bufferLCD);

	lcd_put_cur(0, 7);
	sprintf(bufferLCD, "%s", bufferFreq);
	lcd_send_string(bufferLCD);

	lcd_put_cur(2, 0);
	lcd_send_string("T = ");

	lcd_put_cur(2, 4);
	sprintf(bufferLCD, "%lu", period);
	lcd_send_string(bufferLCD);

	//length = countDigits(period);
	HAL_Delay(10);
	lcd_put_cur(2, (length + 4 + 1));
	lcd_send_string("ciclos");

	lcd_put_cur(3, 0);
	lcd_send_string("Clock = ");

	lcd_put_cur(3, 8);
	sprintf(bufferLCD, "%s", bufferTimerClock);
	lcd_send_string(bufferLCD);
}

void print2USB() {
	#ifdef FREQGENON
		sprintf(bufferCDC,
				"\n\r#########################\r\n"
				"FREQUENCY GENERATOR (Timer 3)\n\r"
				"Frequency: %s\n\r"
				"PSC: %lu\n\rARR: %lu\n\rCCR: %lu\n\r"
				"Duty cycle (CCR / (ARR+1)): %.2f %%\n\r\n\r"
				"Frequency Counter (Timer2)\n\r"
				"Frequency: %s\n\r"
				"Timer Clock: %s\n\rNew CCR: %lu\n\r"
				"Old CCR: %lu\n\rDiff CCR (T in clock pulses): %lu\n\r",
				bufferFreq, PSC, ARR, CCR, dCycle, bufferGenFreq, bufferTimerClock,
					currentCount, lastCountPrint, period);
	#endif
	#ifdef FREQGENOFF
		sprintf(bufferCDC, "########################\r\n"
				"FREQUENCY COUNTER (Timer 2)\r\n"
				"Frequency: %s\r\n"
				"Timer Clock: %s\r\n"
				"New CCR: %lu\r\n"
				"Old CCR: %lu\r\n"
				"Diff CCR (T): %lu\r\n", bufferFreq, bufferTimerClock, currentCount,
				lastCountPrint, period);
	#endif
	CDC_Transmit_FS(bufferCDC, strlen(bufferCDC));
}

void print2BT() {
	#ifdef FREQGENON
		sprintf(bufferBT,
				"#########################\n\r"
				"Frequency Generator (Timer 3)\n"
				"Frequency: %s\n\r"
				"PSC: %lu\n\rARR: %lu\n\rCCR: %lu\n\r"
				"Duty cycle (CCR / (ARR+1)): %.2f %%\n\r\n\r"
				"Frequency Counter (Timer2)\n\r"
				"Frequency: %s\n\r"
				"Timer Clock: %s\n\rNew CCR: %lu\n\r"
				"Old CCR: %lu\n\rDiff CCR (T in clock pulses): \n\r",
				bufferFreq, PSC, ARR, CCR, dCycle, bufferGenFreq, bufferTimerClock,
				currentCount, lastCountPrint, period);
		btSendString(bufferBT);
	#endif
	#ifdef FREQGENOFF
		sprintf(bufferBT,
				"\r\n########################\r\n"
				"FREQUENCY COUNTER (Timer 2)\r\n"
				"Frequency: %s\r\n"
				"Timer Clock: %s\r\n"
				"New CCR: %lu\r\n"
				"Old CCR: %lu\r\n"
				"Diff CCR (T): %lu",
				bufferFreq, bufferTimerClock,
				currentCount, lastCountPrint, period);
		btSendString(bufferBT);
#endif
}

void btSendString(char *Buf) {
	if (flagBT) {
		HAL_UART_Transmit_IT(&huart2, (uint8_t*) Buf, strlen(Buf));
		flagBT = 0;
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		flagBT = 1;
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		currentCount = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		uint32_t arr = TIM3->ARR;
		if (lastCount != 0) {
			if (currentCount >= lastCount) {
				period = currentCount - lastCount;
			} else {
				period = (arr + currentCount) - lastCount;
			}
			frequency = timerClock / (float) period;
		}
		lastCountPrint = lastCount; // Salva em lastCountPrint para mandar pela USB
		lastCount = currentCount;

		flag = 1;
	}
}


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
	while (1) {
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
     ex: printf("Wrong parameters value: file %s on line %d\n\r", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
