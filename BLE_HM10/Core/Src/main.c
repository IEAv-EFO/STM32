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
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 1  // Tamanho do buffer de recepção (um byte por vez)
#define TX_BUFFER_SIZE 50     // Tamanho máximo do buffer para a transmissão
#define RESPONSE_BUFFER_SIZE 10000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t rxBuffer[RX_BUFFER_SIZE];
char assembledBuffer[TX_BUFFER_SIZE] = "";  // Buffer para montar a mensagem recebida
char *command;
char received_char;
char *commands[] = {
    "AT\r\n",
    "AT+RESET\r\n",
    "AT+ROLE0\r\n",
    "AT+RESET\r\n",
    "AT+NAMEBT05\r\n",
    "AT+PIN1234\r\n" // Define o PIN como 1234
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void USART2_Transmit(uint8_t* data, uint16_t size);
extern uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);
void sendATCommand(const uint8_t *command, uint32_t timeOut);
void USART_SendChar(char ch);
char USART_ReceiveChar(void);
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

	HAL_Delay(3000);

	/*
	 * Esse módulo HM-10 já fica automaticamente no modo de comandos AT.
	 * (led piscando). Quando há conexão bluetooth com dispotivo remoto,
	 * o modo de comandos AT fica desabilitado (led acesso continuamente).
	 */

	// Send AT command and receive response to a char buffer
	sendATCommand("AT\r\n", 200); 				// Check if module is responsive
	sendATCommand("AT+BAUD8\r\n", 200); 			// Check if module is responsive
	sendATCommand("AT+NAME\r\n", 200);
	//sendATCommand("AT+NAMEHM10\r\n", 200);
    sendATCommand("AT+TYPE\r\n", 200);
    sendATCommand("AT+ROLE\r\n", 200);
	sendATCommand("AT+HELP\r\n", 5000);
    //sendATCommand("AT+HELP\r\n", 6000);
	//sendATCommand("AT+BAUD8\r\n", 100);// BAUD1=1200, BAUD2=2400, ..., BARD7=57600, BAUD8=115200, ..., BAUD12=132400 bps.

	/*

	 sendATCommand("AT+NAMEBT5\r\n", 100); 	// Set module name
	 sendATCommand("AT+NAME\r\n", 100);			// Query module name
	 sendATCommand("AT+PIN\r\n", 100);
	 sendATCommand("AT+LADDR\r\n", 100);
	 sendATCommand("AT+ROLE\r\n", 100);
	 sendATCommand("AT+BAUD\r\n", 100); 			// Query BaudRate
	 sendATCommand("AT+POWE\r\n", 100);			// Get/Set RF transmit power
	 sendATCommand("AT+UUID\r\n", 100);			//Powers = 0: -23dbm,1: -6dbm, 2: 0dbm, 3: 6dbm
	 sendATCommand("AT+INQ\r\n", 100);
	 sendATCommand("AT+TYPE\r\n", 100);
	 sendATCommand("AT+HELP\r\n", 5000);		    // Query available commands

	 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {

/*		// Check if data is available in USART2 receive buffer
		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == SET) {
			// Read the received character
			received_char = USART_ReceiveChar();

			// Echo back the received character
			USART_SendChar(received_char);
		}*/

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
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  // Habilita a interrupção de recepção (RX)

  HAL_UART_Receive_IT(&huart2, rxBuffer, RX_BUFFER_SIZE);  // Começa a escutar na interrupção

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
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Filtra caracteres de controle ('\n' e '\r')
        if (rxBuffer[0] != '\n' && rxBuffer[0] != '\r') {
            // Adiciona o byte recebido ao buffer de montagem
            size_t len = strlen(assembledBuffer);
            if (len < TX_BUFFER_SIZE - 1) {  // Garante espaço para mais um caractere e o terminador nulo
                assembledBuffer[len] = rxBuffer[0];
                assembledBuffer[len + 1] = '\0';  // Adiciona o terminador nulo
            }
        }

        // Verifica se o final do comando foi atingido, por exemplo, ao receber '\n'
        if (rxBuffer[0] == '\n') {
            // Concatena "_rx" ao comando completo
            if (strlen(assembledBuffer) + 3 < TX_BUFFER_SIZE) {  // Garante espaço para "_rx"
                strncat(assembledBuffer, "_rx", TX_BUFFER_SIZE - strlen(assembledBuffer) - 1);
            }

            // Transmite o comando modificado
            USART2_Transmit((uint8_t *)assembledBuffer, strlen(assembledBuffer));

            // Limpa o buffer montado para a próxima mensagem
            memset(assembledBuffer, 0, TX_BUFFER_SIZE);
        }

        // Reinicia a recepção para o próximo byte
        HAL_UART_Receive_IT(&huart2, rxBuffer, RX_BUFFER_SIZE);
    }
}

// Função para transmitir dados via USART2
void USART2_Transmit(uint8_t* data, uint16_t size) {
    HAL_UART_Transmit(&huart2, data, size, HAL_MAX_DELAY);  // Transmite os dados
}

void sendATCommand(const uint8_t *command, uint32_t timeOut) {
	char response[RESPONSE_BUFFER_SIZE];
	memset(response, 0, RESPONSE_BUFFER_SIZE);

	// Send the AT command
	HAL_UART_Transmit(&huart2, command, strlen(command), timeOut);
	CDC_Transmit_FS(command, strlen(command));

	// Wait for response
	HAL_UART_Receive(&huart2, response, RESPONSE_BUFFER_SIZE, timeOut);
	CDC_Transmit_FS(response, strlen(response));
}

// Send a character via USART
void USART_SendChar(char ch) {
	HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
}

// Receive a character via USART
char USART_ReceiveChar(void) {
	uint8_t ch;
	HAL_UART_Receive(&huart2, &ch, 1, HAL_MAX_DELAY);
	return (char) ch;
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
