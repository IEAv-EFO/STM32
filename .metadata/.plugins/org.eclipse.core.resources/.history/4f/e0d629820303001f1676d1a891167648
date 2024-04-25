/*
* LCD_I2C.c
*
* CRIADO POR ERIK DOS REIS RIBEIRO
* SETEMBRO DE 2023
*
*/
#include "LCD_I2C.h"

#define END_I2C 0x27
#include "main.h"
#include "LCD_I2C.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c1;

void lcd_send_nibble(char nibble)
{
	uint8_t dado_t[2];
	dado_t[0]=(nibble | 0x0C);
	dado_t[1]=(nibble | 0x08);
	HAL_I2C_Master_Transmit(&hi2c1,END_I2C<<1,dado_t, 2, 100);
	
}
void lcd_send_cmd (char cmd)
{

	uint8_t dado_t[4];
	char dado_l, dado_u;
	dado_u=cmd & 0xF0;
	dado_l=(cmd<<4) & 0xF0;
	dado_t[0]=dado_u | 0x0C;
	dado_t[1]=dado_u | 0x08;
	dado_t[2]=dado_l | 0x0C;
	dado_t[3]=dado_l | 0x08;

	HAL_I2C_Master_Transmit(&hi2c1, END_I2C<<1,dado_t, 4, 100);

}
void lcd_send_dado (char dado)
{

	uint8_t dado_t[4];
	char dado_l, dado_u;
	dado_u=dado & 0xF0;
	dado_l=(dado<<4) & 0xF0;
	dado_t[0]=dado_u | 0x0D;
	dado_t[1]=dado_u | 0x09;
	dado_t[2]=dado_l | 0x0D;
	dado_t[3]=dado_l | 0x09;

	HAL_I2C_Master_Transmit(&hi2c1, END_I2C<<1,dado_t, 4, 100);
	
}
void lcd_init (void)
{
	  HAL_Delay(50);
	  lcd_send_nibble(0x30);
	  HAL_Delay(5);
	  lcd_send_nibble(0x30);
	  HAL_Delay(1);
	  lcd_send_nibble(0x30);
	  HAL_Delay(1);
	  lcd_send_nibble(0x20);
	  HAL_Delay(1);

	  lcd_send_cmd(0x28);
	  HAL_Delay(1);
	  lcd_send_cmd(0x06);
	  HAL_Delay(1);
	  lcd_send_cmd(0x0C);
	  HAL_Delay(1);
	  lcd_send_cmd(0x01);
	  HAL_Delay(1);

}

void lcd_send_string(char *str)
{
	while(*str) lcd_send_dado(*str++);
}


void lcd_clear(void)
{
	lcd_send_cmd(0x01);
    HAL_Delay(2);
}

