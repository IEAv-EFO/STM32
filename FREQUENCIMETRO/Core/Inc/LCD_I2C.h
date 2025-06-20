/*
* LCD_I2C.h
*
* CRIADO POR ERIK DOS REIS RIBEIRO
* SETEMBRO DE 2023
*
*/

void lcd_send_cmd (char cmd);
void lcd_send_dado (char dado);
void lcd_init (void);
void lcd_send_string(char *str);
void lcd_send_nibble(char nibble);
void lcd_clear(void);
