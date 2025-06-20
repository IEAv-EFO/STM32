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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DESIRED_BAUDRATE 115200
// Mude aqui a diretiva DESIRED_BAUDRATE
// para a velocidade desejada da UART do
// módulo BLE, compile e grave o firmware
// no STM32

#define RESPONSE_BUFFER_SIZE 100
#define PRINT_BUFFER_SIZE 500
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
const uint16_t timeOut = 200;
uint8_t testAssign;
uint32_t testBR;
uint32_t baudRate[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
char testString[] = "AT\r\n"; // Para imprimir os comandos
							  // aceitos pelo módulo, envie
							  // a string "AT+HELP\r\n"
char baudRateString[10];
char bufferUSB[100];
char rxBuffer[RESPONSE_BUFFER_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
uint32_t detectBaudRate(const char *command, const uint32_t timeout);
uint8_t testBaudRate(uint32_t);
void changeBaudRate();
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
  MX_USB_DEVICE_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(2000);

	testBR = detectBaudRate(testString, timeOut);
	HAL_Delay(250);
	testAssign = testBaudRate(testBR);
	HAL_Delay(250);

	if (testAssign) {
		sprintf(bufferUSB, "Desired baudrate already assigned.\r\n");
		CDC_Transmit_FS(bufferUSB, strlen(bufferUSB));
	}
	else {
		sprintf(bufferUSB, "Changing now to the desired baudrate...\r\n");
		CDC_Transmit_FS(bufferUSB, strlen(bufferUSB));
		changeBaudRate();
		HAL_Delay(250);
		sprintf(bufferUSB, "Testing again...\r\n");
		CDC_Transmit_FS(bufferUSB, strlen(bufferUSB));
		detectBaudRate(testString, timeOut);
	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Function to send an AT command and receive the response
uint32_t detectBaudRate(const char *command, const uint32_t timeOut) {
	uint32_t br;
	char buffer[PRINT_BUFFER_SIZE];
	for (int i = 0; i < 8; i++) {
		br = baudRate[i];
		huart2.Init.BaudRate = baudRate[i];
		HAL_UART_Init(&huart2);
    	memset(rxBuffer, 0, RESPONSE_BUFFER_SIZE);
    	HAL_UART_Transmit(&huart2, (uint8_t *)command, strlen(command), timeOut);
    	HAL_UART_Receive(&huart2, (uint8_t *)rxBuffer, RESPONSE_BUFFER_SIZE, timeOut);
        if (strstr(rxBuffer, "OK") != NULL) {
            snprintf(buffer, PRINT_BUFFER_SIZE, "Baudrate: %ld - Response: %s\r\n", baudRate[i], rxBuffer);
            CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
            return br;
        }
        else {
            char buffer[PRINT_BUFFER_SIZE];
            snprintf(buffer, PRINT_BUFFER_SIZE, "Baudrate: %ld - No response\r\n", baudRate[i]);
            CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
        }
    }
	return br;
}

uint8_t testBaudRate(uint32_t bRate) {
	uint8_t r;
	if (bRate != DESIRED_BAUDRATE) {
		sprintf(bufferUSB, "Desired Baudrate: %lu\r\nCurrent Baudrate: %lu\r\n", DESIRED_BAUDRATE, bRate);
		CDC_Transmit_FS(bufferUSB, strlen(bufferUSB));
		r = 0;
	}
	else {
		r = 1;
	}
	return r;
}

void changeBaudRate() {
	switch (DESIRED_BAUDRATE) {
		case 1200:
			sprintf(baudRateString, "AT+BAUD1\r\n");
			break;
		case 2400:
			sprintf(baudRateString, "AT+BAUD2\r\n");
			break;
		case 4800:
			sprintf(baudRateString, "AT+BAUD3\r\n");
			break;
		case 9600:
			sprintf(baudRateString, "AT+BAUD4\r\n");
			break;
		case 19200:
			sprintf(baudRateString, "AT+BAUD5\r\n");
			break;
		case 38400:
			sprintf(baudRateString, "AT+BAUD6\r\n");
			break;
		case 57600:
			sprintf(baudRateString, "AT+BAUD7\r\n");
			break;
		case 115200:
			sprintf(baudRateString, "AT+BAUD8\r\n");
			break;
		default:
			sprintf(baudRateString, "AT+BAUD4\r\n");
	}
	HAL_UART_Transmit(&huart2, (uint8_t*) baudRateString, strlen(baudRateString), timeOut);
	huart2.Init.BaudRate = DESIRED_BAUDRATE;
	HAL_UART_Init(&huart2);
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
