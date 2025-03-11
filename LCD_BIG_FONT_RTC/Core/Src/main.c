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
 * *
 ******************************************************************************
 */

/* Parte deste código foi adaptada de https://woodsgood.ca/projects/2015/02/17/big-font-lcd-characters/ */
/* e de https://content.instructables.com/FGY/5J1E/GYFYDR5L/FGY5J1EGYFYDR5L.txt */
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
uint8_t col, row, nb = 0, bc = 0;
uint8_t buf[2];
uint8_t bb[8];
char bufferDate[32], bufferTimer[32];
// Tabela com 8 glifos que serão armazenados na CGRAM
const char glifos[][8] = { // Cada linha representa um glyph
		//*	The LL, LT, UB, ect... are rust abreviations to help me
		// designate which segment was which when referencing the large '0'.
				{ 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 }, // glifo LT  LeftTop
				{ 0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F }, // glifo UB  UpperBar
				{ 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x07, 0x03 }, // glifo RT  RightTop
				{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F }, // glifo LL  LowerLeft
				{ 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1E, 0x1C, 0x18 }, // glifo LB  LowerBar
				{ 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x1F }, // glifo LR  LowerRight
				{ 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F }, // glifo UMB UpperMiddleBar
				{ 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F } // glifo LMB LowerMiddleBar
		};

/* A maioria dos caracteres é formada pela combinação de glyphs.
 * Um ou outro caracter (dois pontos :) é chamado do mapa de carateres
 * especiais da CGROM do display. Nesse exemplo (:), é chamado o código A5
 * para a linha superior e também para a linha inferior. 0xA5 é o endereço
 * para um ponto grande no centro do bloco na tabela CGROM do LCD20x4.
 * 0x00 até 0x07 são os endereços voláteis na CGRAM onde foram armazenados os 8 glyphs.
 * 0x20 é o endereço para o espaço na tabela CGROM do LCD20x4.
 * 0xFF é o endereço para o bloco todo acesso na tabela CFROM do LCD20x4.
 * 0x08 é igual ao 0x00.
 */
const char bigChars[][8] = { { 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // Space
		{ 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // !
		{ 0x05, 0x05, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00 }, // "
		{ 0x04, 0xFF, 0x04, 0xFF, 0x04, 0x01, 0xFF, 0x01 }, // #
		{ 0x08, 0xFF, 0x06, 0x07, 0xFF, 0x05, 0x00, 0x00 }, // $
		{ 0x01, 0x20, 0x04, 0x01, 0x04, 0x01, 0x20, 0x04 }, // %
		{ 0x08, 0x06, 0x02, 0x20, 0x03, 0x07, 0x02, 0x04 }, // &
		{ 0x05, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // '
		{ 0x08, 0x01, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00 }, // (
		{ 0x01, 0x02, 0x04, 0x05, 0x00, 0x00, 0x00, 0x00 }, // )
		{ 0x01, 0x04, 0x04, 0x01, 0x04, 0x01, 0x01, 0x04 }, // *
		{ 0x04, 0xFF, 0x04, 0x01, 0xFF, 0x01, 0x00, 0x00 }, // +
		{ 0x20, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, //
		{ 0x04, 0x04, 0x04, 0x20, 0x20, 0x20, 0x00, 0x00 }, // -
		{ 0x20, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // .
		{ 0x20, 0x20, 0x04, 0x01, 0x04, 0x01, 0x20, 0x20 }, // /
		{ 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00 }, // 0
		{ 0x01, 0x02, 0x20, 0x04, 0xFF, 0x04, 0x00, 0x00 }, // 1
		{ 0x06, 0x06, 0x02, 0xFF, 0x07, 0x07, 0x00, 0x00 }, // 2
		{ 0x01, 0x06, 0x02, 0x04, 0x07, 0x05, 0x00, 0x00 }, // 3
		{ 0x03, 0x04, 0xFF, 0x20, 0x20, 0xFF, 0x00, 0x00 }, // 4
		{ 0xFF, 0x06, 0x06, 0x07, 0x07, 0x05, 0x00, 0x00 }, // 5
		{ 0x08, 0x06, 0x06, 0x03, 0x07, 0x05, 0x00, 0x00 }, // 6
		{ 0x01, 0x01, 0x02, 0x20, 0x08, 0x20, 0x00, 0x00 }, // 7
		{ 0x08, 0x06, 0x02, 0x03, 0x07, 0x05, 0x00, 0x00 }, // 8
		{ 0x08, 0x06, 0x02, 0x07, 0x07, 0x05, 0x00, 0x00 }, // 9
		{ 0xA5, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // :
		{ 0x04, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ;
		{ 0x20, 0x04, 0x01, 0x01, 0x01, 0x04, 0x00, 0x00 }, // <
		{ 0x04, 0x04, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00 }, // =
		{ 0x01, 0x04, 0x20, 0x04, 0x01, 0x01, 0x00, 0x00 }, // >
		{ 0x01, 0x06, 0x02, 0x20, 0x07, 0x20, 0x00, 0x00 }, // ?
		{ 0x08, 0x06, 0x02, 0x03, 0x04, 0x04, 0x00, 0x00 }, // @
		{ 0x08, 0x06, 0x02, 0xFF, 0x20, 0xFF, 0x00, 0x00 }, // A
		{ 0xFF, 0x06, 0x05, 0xFF, 0x07, 0x02, 0x00, 0x00 }, // B
		{ 0x08, 0x01, 0x01, 0x03, 0x04, 0x04, 0x00, 0x00 }, // C
		{ 0xFF, 0x01, 0x02, 0xFF, 0x04, 0x05, 0x00, 0x00 }, // D
		{ 0xFF, 0x06, 0x06, 0xFF, 0x07, 0x07, 0x00, 0x00 }, // E
		{ 0xFF, 0x06, 0x06, 0xFF, 0x20, 0x20, 0x00, 0x00 }, // F
		{ 0x08, 0x01, 0x01, 0x03, 0x04, 0x02, 0x00, 0x00 }, // G
		{ 0xFF, 0x04, 0xFF, 0xFF, 0x20, 0xFF, 0x00, 0x00 }, // H
		{ 0x01, 0xFF, 0x01, 0x04, 0xFF, 0x04, 0x00, 0x00 }, // I
		{ 0x20, 0x20, 0xFF, 0x04, 0x04, 0x05, 0x00, 0x00 }, // J
		{ 0xFF, 0x04, 0x05, 0xFF, 0x20, 0x02, 0x00, 0x00 }, // K
		{ 0xFF, 0x20, 0x20, 0xFF, 0x04, 0x04, 0x00, 0x00 }, // L
		{ 0x08, 0x03, 0x05, 0x02, 0xFF, 0x20, 0x20, 0xFF }, // M
		{ 0xFF, 0x02, 0x20, 0xFF, 0xFF, 0x20, 0x03, 0xFF }, // N
		{ 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00 }, // 0
		{ 0x08, 0x06, 0x02, 0xFF, 0x20, 0x20, 0x00, 0x00 }, // P
		{ 0x08, 0x01, 0x02, 0x20, 0x03, 0x04, 0xFF, 0x04 }, // Q
		{ 0xFF, 0x06, 0x02, 0xFF, 0x20, 0x02, 0x00, 0x00 }, // R
		{ 0x08, 0x06, 0x06, 0x07, 0x07, 0x05, 0x00, 0x00 }, // S
		{ 0x01, 0xFF, 0x01, 0x20, 0xFF, 0x20, 0x00, 0x00 }, // T
		{ 0xFF, 0x20, 0xFF, 0x03, 0x04, 0x05, 0x00, 0x00 }, // U
		{ 0x03, 0x20, 0x20, 0x05, 0x20, 0x02, 0x08, 0x20 }, // V
		{ 0xFF, 0x20, 0x20, 0xFF, 0x03, 0x08, 0x02, 0x05 }, // W
		{ 0x03, 0x04, 0x05, 0x08, 0x20, 0x02, 0x00, 0x00 }, // X
		{ 0x03, 0x04, 0x05, 0x20, 0xFF, 0x20, 0x00, 0x00 }, // Y
		{ 0x01, 0x06, 0x05, 0x08, 0x07, 0x04, 0x00, 0x00 }, // Z
		{ 0xFF, 0x01, 0xFF, 0x04, 0x00, 0x00, 0x00, 0x00 }, // [
		{ 0x01, 0x04, 0x20, 0x20, 0x20, 0x20, 0x01, 0x04 }, // Backslash
		{ 0x01, 0xFF, 0x04, 0xFF, 0x00, 0x00, 0x00, 0x00 }, // ]
		{ 0x08, 0x02, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00 }, // ^
		{ 0x20, 0x20, 0x20, 0x04, 0x04, 0x04, 0x00, 0x00 }  // _
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void create_custome_set_of_glyphs();
void create_custom_char(uint8_t loc, uint8_t *data);
void display_custom_char(uint8_t loc);
int writeBigChar(char ch, uint8_t y, uint8_t x);
void writeBigString(char *str, uint8_t y, uint8_t x);
void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void updateTime(void);
const char* Get_WeekDay_String(uint8_t weekDay);
void setDate(uint8_t weekDay, uint8_t month, uint8_t date, uint8_t year);
void updateDate(void);
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
  MX_I2C1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	lcd_init();
	lcd_clear();
	HAL_Delay(100);

	create_custome_set_of_glyphs();

	/* Para visualizar cada glifo (Debug)
	 for (int i = 0; i < 8; i++) {
	 lcd_put_cur(0, i);
	 lcd_send_data(i);
	 HAL_Delay(250);
	 }
	 */

// Inicialização
	lcd_clear();

	lcd_send_string(0, 0, "EFO-S");

	setDate(RTC_WEEKDAY_FRIDAY, RTC_MONTH_FEBRUARY, 07, 25);
	setTime(8, 37, 0);

	HAL_Delay(200);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		// Dessa forma sem usar ISR (interrupção/callback)
		/*
		 updateDate();
		 lcd_put_cur(0, 10);
		 lcd_send_string(bufferDate);

		 updateTime();
		 writeBigString(bufferTimer, 2, 0);
		 HAL_Delay(300);

		 HAL_Delay(1000);
		 */

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 96;
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
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void create_custome_set_of_glyphs() {
	for (nb = 0; nb < 8; nb++) { // Cria os 8 glifos usandos para formar o caracter
		for (bc = 0; bc < 8; bc++) {
			bb[bc] = glifos[nb][bc];
		}
		create_custom_char(nb + 1, bb); // Cria e armazena os glifos na CGRAM
	}
}

void create_custom_char(uint8_t loc, uint8_t *data) {
	switch (loc) {
	case 0:
		lcd_send_cmd(0x40);
		break;
	case 1:
		lcd_send_cmd(0x40 + 8);
		break;
	case 2:
		lcd_send_cmd(0x40 + 16);
		break;
	case 3:
		lcd_send_cmd(0x40 + 24);
		break;
	case 4:
		lcd_send_cmd(0x40 + 32);
		break;
	case 5:
		lcd_send_cmd(0x40 + 40);
		break;
	case 6:
		lcd_send_cmd(0x40 + 48);
		break;
	case 7:
		lcd_send_cmd(0x40 + 56);
		break;
	default:
		break;
	}

	uint8_t cc_data[8];
	for (int i = 0; i < 8; i++) {
		cc_data[i] = *data++;
	}

	for (int i = 0; i < 8; i++) {
		lcd_send_data(cc_data[i]);
	}

}

void display_custom_char(uint8_t loc) {
	switch (loc) {
	case 0:
		lcd_send_data(0);
		break;
	case 1:
		lcd_send_data(1);
		break;
	case 2:
		lcd_send_data(2);
		break;
	case 3:
		lcd_send_data(3);
		break;
	case 4:
		lcd_send_data(4);
		break;
	case 5:
		lcd_send_data(5);
		break;
	case 6:
		lcd_send_data(6);
		break;
	case 7:
		lcd_send_data(7);
		break;
	default:
		break;
	}
}

/* Código adaptado de https://woodsgood.ca/projects/2015/02/17/big-font-lcd-characters/ */
// writeBigChar: writes big character 'ch' to column x, row y; returns number of columns used by 'ch'
int writeBigChar(char ch, uint8_t x, uint8_t y) {
	if (ch < ' ' || ch > '_')
		return 0;               // If outside table range, do nothing
	nb = 0;                     // character byte counter
	for (bc = 0; bc < 8; bc++) {
		bb[bc] = bigChars[ch - ' '][bc];
		if (bb[bc] != 0)
			nb++;
	}

	bc = 0;
	for (row = y; row < y + 2; row++) {
		for (col = x; col < x + nb / 2; col++) {
			lcd_put_cur(row, col);
			lcd_send_data(bb[bc++]);
		}
//    lcd_put_cur(row, col);
//    lcd_send_string(' ');         // Write ' ' between letters
	}
	return nb / 2 - 1;              // returns number of columns used by char
}

void writeBigString(char *str, uint8_t y, uint8_t x) {
	char c;
	while ((c = *str++)) {
		x += writeBigChar(c, x, y) + 1;
	}

/*	 // Teste para mover os caracteres. Não está legal ainda.
	 char *inStr = str; uint8_t inY = y; uint8_t inX = x;
	 uint8_t flagMov = 1;
	 while (flagMov) {
		 while (c = *inStr++) {
			 inX += writeBigChar(c, inX, inY) + 1;
		 }
		 HAL_Delay(400);
		 lcd_clear();
		 inX += x + 1;
		 inStr = str;
		 if (inX > 17) {
			 inX = x;
			 lcd_clear();
		 }
	 }*/
}

void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
	RTC_TimeTypeDef sTime = { 0 };

	sTime.Hours = hours;
	sTime.Minutes = minutes;
	sTime.Seconds = seconds;
	sTime.TimeFormat = RTC_HOURFORMAT_24;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}
}

void updateTime(void) {
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

	sprintf(bufferTimer, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
}

const char* Get_WeekDay_String(uint8_t weekDay) {
	switch (weekDay) {
		case RTC_WEEKDAY_MONDAY:
			return "Seg";
		case RTC_WEEKDAY_TUESDAY:
			return "Ter";
		case RTC_WEEKDAY_WEDNESDAY:
			return "Qua";
		case RTC_WEEKDAY_THURSDAY:
			return "Qui";
		case RTC_WEEKDAY_FRIDAY:
			return "Sex";
		case RTC_WEEKDAY_SATURDAY:
			return "Sab";
		case RTC_WEEKDAY_SUNDAY:
			return "Dom";
		default:
			return "Unk";
	}
}

void setDate(uint8_t weekDay, uint8_t month, uint8_t date, uint8_t year) {
	RTC_DateTypeDef sDate = { 0 };

	sDate.WeekDay = weekDay;
	sDate.Month = month;
	sDate.Date = date;
	sDate.Year = year;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}
}

void updateDate(void) {
	RTC_DateTypeDef sDate;

	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}

    const char* weekDayString = Get_WeekDay_String(sDate.WeekDay);
    sprintf(bufferDate, "%02d/%02d/%04d %s", sDate.Date, sDate.Month, 2000 + sDate.Year, weekDayString);
}

// Interrupção callback ocorre de 1 em 1s (1 Hz) configurado no CubeMX
// no timer RTC. Deve-se habilar a opção WakeUp para Internal WakeUp
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {
	updateDate();
	updateTime();

	lcd_send_string(0, 6, bufferDate);
	writeBigString(bufferTimer, 2, 0);
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
