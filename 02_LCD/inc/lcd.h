#ifndef LCD_H
#define LCD_H

void lcd_command_write( unsigned char command );
void set_lcd_port_output( void );
void lcd_clear( void);
int lcd_gotoxy( unsigned int x, unsigned int y);
void lcd_data_write( unsigned char data );
void lcd_putchar( int c );
void lcd_puts(char *string );
void lcd_init( void );
#endif
