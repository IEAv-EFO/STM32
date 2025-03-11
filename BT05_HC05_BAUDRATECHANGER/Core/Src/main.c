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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PRINT_BUFFER_SIZE 500
#define RESPONSE_BUFFER_SIZE 100
//#define BT05
//#define HC05
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
GPIO_PinState pa4, pa5;
uint8_t flag0 = 0;  // Pushbutton "KEY" no STM32
uint8_t flag4 = 0;  // BT05
uint8_t flag5 = 0;  // HC05
uint8_t flag6 = 0;  // Pushbutton 1 no protoboard
uint8_t flag7 = 0;  // Pushbutton 2 no protoboard
uint8_t flag8 = 0;  // Pushbutton 3 no protoboard
uint8_t flag9 = 0;  // Pushbutton 4 no protoboard
uint8_t flag10 = 0; // Pushbutton 5 no protoboard
uint8_t timeOut = 200;
uint32_t detectedBR;
char rxBuffer[100];
char testBT05[] = "AT\r\n"; // O BT05 HM-10 retorna "OK"
char testHC05[] = "AT+UART?\r\n";
uint32_t baudRates[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };

char *BT05_baudRateCodes[] = {
		"AT+BAUD1\r\n", // 1200
		"AT+BAUD2\r\n", // 2400
		"AT+BAUD3\r\n", // 4800
		"AT+BAUD4\r\n", // 9600   Botão 1 PA6 Normalmente padrão
		"AT+BAUD5\r\n", // 19200  Botão 2 PA7
		"AT+BAUD6\r\n", // 38400  Botão 3 PA8
		"AT+BAUD7\r\n", // 57600  Botão 4 PA9
		"AT+BAUD8\r\n"  // 115200 Botão 5 PA10
		};
//#endif

char *HC05_baudRateCodes[] = {
		"AT+UART=1200,0,0\r\n", // 1200
		"AT+UART=2400,0,0\r\n", // 2400
		"AT+UART=4800,0,0\r\n", // 4800
		"AT+UART=9600,0,0\r\n", // 9600   Botão 1 PA6
		"AT+UART=19200,0,0\r\n", // 19200  Botão 2 PA7
		"AT+UART=38400,0,0\r\n", // 38400  Botão 3 PA8 Normalmente padrão
		"AT+UART=57600,0,0\r\n", // 57600  Botão 4 PA9
		"AT+UART=115200,0,0\r\n"  // 115200 Botão 5 PA10
		};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
uint32_t detectBaudRate();
void changeBaudRate(uint8_t bRateCode, char *Buf);
void blinkTest(uint8_t, uint16_t);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	pa4 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	pa5 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);

	if (pa4 == GPIO_PIN_SET && pa5 == GPIO_PIN_RESET) {
		flag4 = 1;
		flag5 = 0;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	}
	if (pa4 == GPIO_PIN_RESET && pa5 == GPIO_PIN_SET) {
		flag4 = 0;
		flag5 = 1;
		huart2.Init.BaudRate = 38400;
		HAL_UART_Init(&huart2);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		if (flag0) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
			// LED apagado indicando mode de alteração
			// Se flag0 então pode-se alterar a baudrate
			if (flag6) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[3]) {
					blinkTest(10, 50); // Pisca rápido indicando que a baudrate selecionda
				}                      // já é a baudrate atual do BT05
				else {
					if (flag5 == 1) {
						changeBaudRate(3, HC05_baudRateCodes[3]); // "AT+UART=9600,0,0\r\n" 9600
					}
					if (flag4 == 1) {
						changeBaudRate(3, BT05_baudRateCodes[3]); // "AT+BAUD4\r\n" 9600
					}
					HAL_Delay(200);
					detectedBR = detectBaudRate();
					if (detectedBR == baudRates[3]) {
						blinkTest(4, 300);
					}
					else {
						blinkTest(10, 50); // Pisca rápido indicando que a baudrate selecionda
					}					   // já é a baudrate atual do BT05
				}
				NVIC_SystemReset();
			}
			if (flag7) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[4]) {
					blinkTest(10, 50);
				}
				else {
					if (flag5 == 1) {
						changeBaudRate(4, HC05_baudRateCodes[4]); // "AT+UART=9600,0,0\r\n" 9600
					}
					if (flag4 == 1) {
						changeBaudRate(4, BT05_baudRateCodes[4]); // "AT+BAUD4\r\n" 9600
					}
					HAL_Delay(200);
					detectedBR = detectBaudRate();
					if (detectedBR == baudRates[4]) {
						blinkTest(5, 300);
					}
					else {
						blinkTest(10, 50);
					}
				}
				NVIC_SystemReset();
			}
			if (flag8) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[5]) {
					blinkTest(10, 50);
				}
				else {
					if (flag5 == 1) {
						changeBaudRate(5, HC05_baudRateCodes[5]); // "AT+UART=9600,0,0\r\n" 9600
					}
					if (flag4 == 1) {
						changeBaudRate(5, BT05_baudRateCodes[5]); // "AT+BAUD4\r\n" 9600
					}
						HAL_Delay(200);
					detectedBR = detectBaudRate();
					if (detectedBR == baudRates[5]) {
						blinkTest(6, 300);
					}
					else {
						blinkTest(10, 50);
					}
				}
				NVIC_SystemReset();
			}
			if (flag9) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[6]) {
					blinkTest(10, 50);
				}
				else {
					if (flag5 == 1) {
						changeBaudRate(6, HC05_baudRateCodes[6]); // "AT+UART=9600,0,0\r\n" 9600
					}
					if (flag4 == 1) {
						changeBaudRate(6, BT05_baudRateCodes[6]); // "AT+BAUD4\r\n" 9600
					}
					HAL_Delay(200);
					detectedBR = detectBaudRate();
					if (detectedBR == baudRates[6]) {
						blinkTest(7, 300);
					}
					else {
						blinkTest(10, 50);
					}
				}
				NVIC_SystemReset();
			}
			if (flag10) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[7]) {
					blinkTest(10, 50);
				}
				else {
					if (flag5 == 1) {
						changeBaudRate(7, HC05_baudRateCodes[7]); // "AT+UART=9600,0,0\r\n" 9600
					}
					if (flag4 == 1) {
						changeBaudRate(7, BT05_baudRateCodes[7]); // "AT+BAUD4\r\n" 9600
					}
					HAL_Delay(200);
					detectedBR = detectBaudRate();
					if (detectedBR == baudRates[7]) {
						blinkTest(8, 300);
					}
					else {
						blinkTest(10, 50);
					}
				}
				NVIC_SystemReset();
			}

		}
		else {
			// Não se pode alterar a baudrate.
			// Somente a verificação está habilitada.
			if (flag6) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[3]) {
					blinkTest(4, 300);
				} else {
					blinkTest(10, 50); // Pisca rápido indicando que a baudrate selecionda
				}                      // já é a baudrate atual do BT05
				flag6 = 0;
			}

			if (flag7) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[4]) {
					blinkTest(5, 300);
				} else {
					blinkTest(10, 50);
				}
				flag7 = 0;
			}

			if (flag8) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[5]) {
					blinkTest(6, 300);
				} else {
					blinkTest(10, 50);
				}
				flag8 = 0;
			}

			if (flag9) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[6]) {
					blinkTest(7, 300);
				} else {
					blinkTest(10, 50);
				}
				flag9 = 0;
			}

			if (flag10) {
				detectedBR = detectBaudRate();
				if (detectedBR == baudRates[7]) {
					blinkTest(8, 300);
				} else {
					blinkTest(10, 50);
				}
				flag10 = 0;
			}
		}

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
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
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
  huart2.Init.BaudRate = 9600;
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA6 PA7 PA8
                           PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

uint32_t detectBaudRate() {
	int br;
	if (flag5 == 1) {
		for (int i = 0; i < 8; i++) {
			br = baudRates[i];
			char str[32];
			memset(rxBuffer, 0, sizeof(rxBuffer));
			HAL_UART_Transmit(&huart2, (uint8_t*) testHC05, strlen(testHC05), timeOut);
			HAL_UART_Receive(&huart2, (uint8_t*) rxBuffer, RESPONSE_BUFFER_SIZE, timeOut);
			sprintf(str, "%d", br);
			if (strstr(rxBuffer, str) != NULL) {
				// Se na string rxBuffer existir um substring "OK"
				return br;
			}
			br = 0;
		}
	}
	if (flag4 == 1) {
		for (int i = 0; i < 8; i++) {
			br = baudRates[i];
			huart2.Init.BaudRate = baudRates[i];
			HAL_UART_Init(&huart2);
			memset(rxBuffer, 0, sizeof(rxBuffer));
			HAL_UART_Transmit(&huart2, (uint8_t*) testBT05, strlen(testBT05), timeOut);
			HAL_UART_Receive(&huart2, (uint8_t*) rxBuffer, RESPONSE_BUFFER_SIZE, timeOut);
			if (strstr(rxBuffer, "OK") != NULL) {
				// Se na string rxBuffer existir um substring "OK"
				return br;
			}
			br = 0;
		}
	}

	return br;
}

void changeBaudRate(uint8_t bRateCode, char *Buf) {
	HAL_UART_Transmit(&huart2, (uint8_t*) Buf, strlen(Buf), timeOut);
	if (flag4 == 1) {
		switch (bRateCode) {
			case 0:
				huart2.Init.BaudRate = baudRates[0];
				break;
			case 1:
				huart2.Init.BaudRate = baudRates[1];
				break;
			case 2:
				huart2.Init.BaudRate = baudRates[2];
				break;
			case 3:
				huart2.Init.BaudRate = baudRates[3];
				break;
			case 4:
				huart2.Init.BaudRate = baudRates[4];
				break;
			case 5:
				huart2.Init.BaudRate = baudRates[5];
				break;
			case 6:
				huart2.Init.BaudRate = baudRates[6];
				break;
			case 7:
				huart2.Init.BaudRate = baudRates[7];
				break;
			default:
				huart2.Init.BaudRate = baudRates[3];
				break;
		}
	}
	HAL_UART_Init(&huart2);
 }

void blinkTest(uint8_t bCode, uint16_t dly) {
	uint8_t nBlink;
	HAL_Delay(dly);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	for (uint8_t i = 0; i < 2 * bCode; i++) {
		// Multiplica por 2 porque são nBlink vezes acesos e nBlink vezes apagado
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(dly);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case GPIO_PIN_0: // Alterna entre alteração ou somente verificação da baudrate
		flag0 = 1;
		break;
	case GPIO_PIN_4:
		flag4 = 1;
		flag5 = 0;
		huart2.Init.BaudRate = 9600;
		HAL_UART_Init(&huart2);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		break;
	case GPIO_PIN_5:
		flag4 = 0;
		flag5 = 1;
		huart2.Init.BaudRate = 38400;
		HAL_UART_Init(&huart2);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		break;
	case GPIO_PIN_6: // PA6 9600
		flag6 = 1;
		break;
	case GPIO_PIN_7: // PA7 19200
		flag7 = 1;
		break;
	case GPIO_PIN_8: // PA8 38400
		flag8 = 1;
		break;
	case GPIO_PIN_9: // PA9 57600
		flag9 = 1;
		break;
	case GPIO_PIN_10: // PA10 115200
		flag10 = 1;
		break;
	default:
		break;
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */







// main.c completo escrito para o HC05 e BT05, mas com seleção via diretivas
// de compilação: #define BT05 e #define HC05.
// Não há necessidade de alterar os dispositivos no protoboard.


//
//
//
///* USER CODE BEGIN Header */
///**
// ******************************************************************************
// * @file           : main.c
// * @brief          : Main program body
// ******************************************************************************
// * @attention
// *
// * Copyright (c) 2024 STMicroelectronics.
// * All rights reserved.
// *
// * This software is licensed under terms that can be found in the LICENSE file
// * in the root directory of this software component.
// * If no LICENSE file comes with this software, it is provided AS-IS.
// *
// ******************************************************************************
// */
///* USER CODE END Header */
///* Includes ------------------------------------------------------------------*/
//#include "main.h"
//
///* Private includes ----------------------------------------------------------*/
///* USER CODE BEGIN Includes */
//#include <string.h>
///* USER CODE END Includes */
//
///* Private typedef -----------------------------------------------------------*/
///* USER CODE BEGIN PTD */
//
///* USER CODE END PTD */
//
///* Private define ------------------------------------------------------------*/
///* USER CODE BEGIN PD */
//#define PRINT_BUFFER_SIZE 500
//#define RESPONSE_BUFFER_SIZE 100
////#define BT05
//#define HC05
///* USER CODE END PD */
//
///* Private macro -------------------------------------------------------------*/
///* USER CODE BEGIN PM */
//
///* USER CODE END PM */
//
///* Private variables ---------------------------------------------------------*/
//UART_HandleTypeDef huart2;
//
///* USER CODE BEGIN PV */
//uint8_t flag0 = 0;  // Pushbutton "KEY" no STM32
//uint8_t flag6 = 0;  // Pushbutton 1 no protoboard
//uint8_t flag7 = 0;  // Pushbutton 2 no protoboard
//uint8_t flag8 = 0;  // Pushbutton 3 no protoboard
//uint8_t flag9 = 0;  // Pushbutton 4 no protoboard
//uint8_t flag10 = 0; // Pushbutton 5 no protoboard
//uint8_t timeOut = 200;
//uint32_t detectedBR;
//char rxBuffer[100];
//char testBT05[] = "AT\r\n"; // O BT05 HM-10 retorna "OK"
//char testHC05[] = "AT+UART?\r\n";
//uint32_t baudRates[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
//
//#ifdef BT05
//char *BT05_baudRateCodes[] = {
//		"AT+BAUD1\r\n", // 1200
//		"AT+BAUD2\r\n", // 2400
//		"AT+BAUD3\r\n", // 4800
//		"AT+BAUD4\r\n", // 9600   Botão 1 PA6 Normalmente padrão
//		"AT+BAUD5\r\n", // 19200  Botão 2 PA7
//		"AT+BAUD6\r\n", // 38400  Botão 3 PA8
//		"AT+BAUD7\r\n", // 57600  Botão 4 PA9
//		"AT+BAUD8\r\n"  // 115200 Botão 5 PA10
//		};
//#endif
//
//#ifdef HC05
//char *HC05_baudRateCodes[] = {
//		"AT+UART=1200,0,0\r\n", // 1200
//		"AT+UART=2400,0,0\r\n", // 2400
//		"AT+UART=4800,0,0\r\n", // 4800
//		"AT+UART=9600,0,0\r\n", // 9600   Botão 1 PA6
//		"AT+UART=19200,0,0\r\n", // 19200  Botão 2 PA7
//		"AT+UART=38400,0,0\r\n", // 38400  Botão 3 PA8 Normalmente padrão
//		"AT+UART=57600,0,0\r\n", // 57600  Botão 4 PA9
//		"AT+UART=115200,0,0\r\n"  // 115200 Botão 5 PA10
//		};
//#endif
///* USER CODE END PV */
//
///* Private function prototypes -----------------------------------------------*/
//void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
//static void MX_USART2_UART_Init(void);
///* USER CODE BEGIN PFP */
//uint32_t detectBaudRate();
//void changeBaudRate(uint8_t bRateCode, char *Buf);
//void blinkTest(uint8_t, uint16_t);
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
//
///* USER CODE END PFP */
//
///* Private user code ---------------------------------------------------------*/
///* USER CODE BEGIN 0 */
//
///* USER CODE END 0 */
//
///**
//  * @brief  The application entry point.
//  * @retval int
//  */
//int main(void)
//{
//
//  /* USER CODE BEGIN 1 */
//
//  /* USER CODE END 1 */
//
//  /* MCU Configuration--------------------------------------------------------*/
//
//  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//  HAL_Init();
//
//  /* USER CODE BEGIN Init */
//
//  /* USER CODE END Init */
//
//  /* Configure the system clock */
//  SystemClock_Config();
//
//  /* USER CODE BEGIN SysInit */
//
//  /* USER CODE END SysInit */
//
//  /* Initialize all configured peripherals */
//  MX_GPIO_Init();
//  MX_USART2_UART_Init();
//  /* USER CODE BEGIN 2 */
//	HAL_Delay(1000);
//
//  /* USER CODE END 2 */
//
//  /* Infinite loop */
//  /* USER CODE BEGIN WHILE */
//	while (1) {
//
//		if (flag0) {
//			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
//			// LED apagado indicando mode de alteração
//			// Se flag0 então pode-se alterar a baudrate
//			if (flag6) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[3]) {
//					blinkTest(10, 50); // Pisca rápido indicando que a baudrate selecionda
//				}                      // já é a baudrate atual do BT05
//				else {
//					#ifdef HC05
//						changeBaudRate(3, HC05_baudRateCodes[3]); // "AT+UART=9600,0,0\r\n" 9600
//					#endif
//					#ifdef BT05
//						changeBaudRate(3, BT05_baudRateCodes[3]); // "AT+BAUD4\r\n" 9600
//					#endif
//					HAL_Delay(200);
//					detectedBR = detectBaudRate();
//					if (detectedBR == baudRates[3]) {
//						blinkTest(4, 300);
//					}
//					else {
//						blinkTest(10, 50); // Pisca rápido indicando que a baudrate selecionda
//					}					   // já é a baudrate atual do BT05
//				}
//				NVIC_SystemReset();
//			}
//			if (flag7) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[4]) {
//					blinkTest(10, 50);
//				}
//				else {
//					#ifdef HC05
//						changeBaudRate(4, HC05_baudRateCodes[4]); // "AT+UART=9600,0,0\r\n" 9600
//					#endif
//					#ifdef BT05
//						changeBaudRate(4, BT05_baudRateCodes[4]); // "AT+BAUD4\r\n" 9600
//					#endif
//					HAL_Delay(200);
//					detectedBR = detectBaudRate();
//					if (detectedBR == baudRates[4]) {
//						blinkTest(5, 300);
//					}
//					else {
//						blinkTest(10, 50);
//					}
//				}
//				NVIC_SystemReset();
//			}
//			if (flag8) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[5]) {
//					blinkTest(10, 50);
//				}
//				else {
//					#ifdef HC05
//						changeBaudRate(5, HC05_baudRateCodes[5]); // "AT+UART=9600,0,0\r\n" 9600
//					#endif
//					#ifdef BT05
//						changeBaudRate(5, BT05_baudRateCodes[5]); // "AT+BAUD4\r\n" 9600
//					#endif
//						HAL_Delay(200);
//					detectedBR = detectBaudRate();
//					if (detectedBR == baudRates[5]) {
//						blinkTest(6, 300);
//					}
//					else {
//						blinkTest(10, 50);
//					}
//				}
//				NVIC_SystemReset();
//			}
//			if (flag9) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[6]) {
//					blinkTest(10, 50);
//				}
//				else {
//					#ifdef HC05
//						changeBaudRate(6, HC05_baudRateCodes[6]); // "AT+UART=9600,0,0\r\n" 9600
//					#endif
//					#ifdef BT05
//						changeBaudRate(6, BT05_baudRateCodes[6]); // "AT+BAUD4\r\n" 9600
//					#endif
//					HAL_Delay(200);
//					detectedBR = detectBaudRate();
//					if (detectedBR == baudRates[6]) {
//						blinkTest(7, 300);
//					}
//					else {
//						blinkTest(10, 50);
//					}
//				}
//				NVIC_SystemReset();
//			}
//			if (flag10) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[7]) {
//					blinkTest(10, 50);
//				}
//				else {
//					#ifdef HC05
//						changeBaudRate(7, HC05_baudRateCodes[7]); // "AT+UART=9600,0,0\r\n" 9600
//					#endif
//					#ifdef BT05
//						changeBaudRate(7, BT05_baudRateCodes[7]); // "AT+BAUD4\r\n" 9600
//					#endif
//					HAL_Delay(200);
//					detectedBR = detectBaudRate();
//					if (detectedBR == baudRates[7]) {
//						blinkTest(8, 300);
//					}
//					else {
//						blinkTest(10, 50);
//					}
//				}
//				NVIC_SystemReset();
//			}
//
//		}
//		else {
//			// Não se pode alterar a baudrate.
//			// Somente a verificação está habilitada.
//			if (flag6) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[3]) {
//					blinkTest(4, 300);
//				} else {
//					blinkTest(10, 50); // Pisca rápido indicando que a baudrate selecionda
//				}                      // já é a baudrate atual do BT05
//				flag6 = 0;
//			}
//
//			if (flag7) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[4]) {
//					blinkTest(5, 300);
//				} else {
//					blinkTest(10, 50);
//				}
//				flag7 = 0;
//			}
//
//			if (flag8) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[5]) {
//					blinkTest(6, 300);
//				} else {
//					blinkTest(10, 50);
//				}
//				flag8 = 0;
//			}
//
//			if (flag9) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[6]) {
//					blinkTest(7, 300);
//				} else {
//					blinkTest(10, 50);
//				}
//				flag9 = 0;
//			}
//
//			if (flag10) {
//				detectedBR = detectBaudRate();
//				if (detectedBR == baudRates[7]) {
//					blinkTest(8, 300);
//				} else {
//					blinkTest(10, 50);
//				}
//				flag10 = 0;
//			}
//		}
//
//    /* USER CODE END WHILE */
//
//    /* USER CODE BEGIN 3 */
//	}
//  /* USER CODE END 3 */
//}
//
///**
//  * @brief System Clock Configuration
//  * @retval None
//  */
//void SystemClock_Config(void)
//{
//  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//
//  /** Configure the main internal regulator output voltage
//  */
//  __HAL_RCC_PWR_CLK_ENABLE();
//  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//
//  /** Initializes the RCC Oscillators according to the specified parameters
//  * in the RCC_OscInitTypeDef structure.
//  */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
//  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
//  RCC_OscInitStruct.PLL.PLLM = 8;
//  RCC_OscInitStruct.PLL.PLLN = 100;
//  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//  RCC_OscInitStruct.PLL.PLLQ = 5;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Initializes the CPU, AHB and APB buses clocks
//  */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//
//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}
//
///**
//  * @brief USART2 Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_USART2_UART_Init(void)
//{
//
//  /* USER CODE BEGIN USART2_Init 0 */
//
//  /* USER CODE END USART2_Init 0 */
//
//  /* USER CODE BEGIN USART2_Init 1 */
//
//  /* USER CODE END USART2_Init 1 */
//  huart2.Instance = USART2;
//#ifdef HC05
//  huart2.Init.BaudRate = 38400;
//#endif
//#ifdef BT05
//  huart2.Init.BaudRate = 9600;
//#endif
//  huart2.Init.WordLength = UART_WORDLENGTH_8B;
//  huart2.Init.StopBits = UART_STOPBITS_1;
//  huart2.Init.Parity = UART_PARITY_NONE;
//  huart2.Init.Mode = UART_MODE_TX_RX;
//  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
//  if (HAL_UART_Init(&huart2) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN USART2_Init 2 */
//
//  /* USER CODE END USART2_Init 2 */
//
//}
//
///**
//  * @brief GPIO Initialization Function
//  * @param None
//  * @retval None
//  */
//static void MX_GPIO_Init(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
///* USER CODE BEGIN MX_GPIO_Init_1 */
///* USER CODE END MX_GPIO_Init_1 */
//
//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOH_CLK_ENABLE();
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//
//  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
//
//  /*Configure GPIO pin : PC13 */
//  GPIO_InitStruct.Pin = GPIO_PIN_13;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//
//  /*Configure GPIO pins : PA0 PA6 PA7 PA8
//                           PA9 PA10 */
//  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
//                          |GPIO_PIN_9|GPIO_PIN_10;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : PA4 */
//  GPIO_InitStruct.Pin = GPIO_PIN_4;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//  /*Configure GPIO pin : PA5 */
//  GPIO_InitStruct.Pin = GPIO_PIN_5;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//  /* EXTI interrupt init*/
//  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
//
//  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
//
//  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//
///* USER CODE BEGIN MX_GPIO_Init_2 */
///* USER CODE END MX_GPIO_Init_2 */
//}
//
///* USER CODE BEGIN 4 */
//
//uint32_t detectBaudRate() {
//	int br;
//#ifdef HC05
//	for (int i = 0; i < 8; i++) {
//		br = baudRates[i];
//		char str[32];
//		memset(rxBuffer, 0, sizeof(rxBuffer));
//		HAL_UART_Transmit(&huart2, (uint8_t*) testHC05, strlen(testHC05), timeOut);
//		HAL_UART_Receive(&huart2, (uint8_t*) rxBuffer, RESPONSE_BUFFER_SIZE, timeOut);
//		sprintf(str, "%d", br);
//		if (strstr(rxBuffer, str) != NULL) {
//			// Se na string rxBuffer existir um substring "OK"
//			return br;
//		}
//		br = 0;
//	}
//#endif
//
//#ifdef BT05
//	for (int i = 0; i < 8; i++) {
//		br = baudRates[i];
//		huart2.Init.BaudRate = baudRates[i];
//		HAL_UART_Init(&huart2);
//		memset(rxBuffer, 0, sizeof(rxBuffer));
//		HAL_UART_Transmit(&huart2, (uint8_t*) testBT05, strlen(testBT05), timeOut);
//		HAL_UART_Receive(&huart2, (uint8_t*) rxBuffer, RESPONSE_BUFFER_SIZE, timeOut);
//		if (strstr(rxBuffer, "OK") != NULL) {
//			// Se na string rxBuffer existir um substring "OK"
//			return br;
//		}
//		br = 0;
//	}
//#endif
//	return br;
//}
//
//void changeBaudRate(uint8_t bRateCode, char *Buf) {
//	HAL_UART_Transmit(&huart2, (uint8_t*) Buf, strlen(Buf), timeOut);
//#ifdef BT05
//	switch (bRateCode) {
//		case 0:
//			huart2.Init.BaudRate = baudRates[0];
//			break;
//		case 1:
//			huart2.Init.BaudRate = baudRates[1];
//			break;
//		case 2:
//			huart2.Init.BaudRate = baudRates[2];
//			break;
//		case 3:
//			huart2.Init.BaudRate = baudRates[3];
//			break;
//		case 4:
//			huart2.Init.BaudRate = baudRates[4];
//			break;
//		case 5:
//			huart2.Init.BaudRate = baudRates[5];
//			break;
//		case 6:
//			huart2.Init.BaudRate = baudRates[6];
//			break;
//		case 7:
//			huart2.Init.BaudRate = baudRates[7];
//			break;
//		default:
//			huart2.Init.BaudRate = baudRates[3];
//			break;
//	}
//	HAL_UART_Init(&huart2);
//#endif
// }
//
//void blinkTest(uint8_t bCode, uint16_t dly) {
//	uint8_t nBlink;
//	HAL_Delay(dly);
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
//	for (uint8_t i = 0; i < 2 * bCode; i++) {
//		// Multiplica por 2 porque são nBlink vezes acesos e nBlink vezes apagado
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//		HAL_Delay(dly);
//	}
//}
//
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//	switch (GPIO_Pin) {
//	case GPIO_PIN_0: // Alterna entre alteração ou somente verificação da baudrate
//		flag0 = 1;
//		break;
//	case GPIO_PIN_6: // PA6 9600
//		flag6 = 1;
//		break;
//	case GPIO_PIN_7: // PA7 19200
//		flag7 = 1;
//		break;
//	case GPIO_PIN_8: // PA8 38400
//		flag8 = 1;
//		break;
//	case GPIO_PIN_9: // PA9 57600
//		flag9 = 1;
//		break;
//	case GPIO_PIN_10: // PA10 115200
//		flag10 = 1;
//		break;
//	default:
//		break;
//	}
//}
//
///* USER CODE END 4 */
//
///**
//  * @brief  This function is executed in case of error occurrence.
//  * @retval None
//  */
//void Error_Handler(void)
//{
//  /* USER CODE BEGIN Error_Handler_Debug */
//	/* User can add his own implementation to report the HAL error return state */
//	__disable_irq();
//	while (1) {
//	}
//  /* USER CODE END Error_Handler_Debug */
//}
//
//#ifdef  USE_FULL_ASSERT
///**
//  * @brief  Reports the name of the source file and the source line number
//  *         where the assert_param error has occurred.
//  * @param  file: pointer to the source file name
//  * @param  line: assert_param error line source number
//  * @retval None
//  */
//void assert_failed(uint8_t *file, uint32_t line)
//{
//  /* USER CODE BEGIN 6 */
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
//  /* USER CODE END 6 */
//}
//#endif /* USE_FULL_ASSERT */
