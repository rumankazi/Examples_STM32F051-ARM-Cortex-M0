#include "stm32f0xx_conf.h"

#include "lcd.h"
#include "delay.h"

#include <stdint.h>

//#define GPIOB_BRR *(volatile uint16_t *) (GPIOB_BASE + 0x1A)   /* Port B set reset register */
 
#define LCD_DATA_DIR	   GPIOC->MODER
#define LCD_DATA_SET	   GPIOC->BSRR
#define LCD_DATA_CLR	   GPIOC->BRR

#define LCD_CTRL_DIR	   GPIOB->MODER
#define LCD_CTRL_SET     GPIOB->BSRR
#define LCD_CTRL_CLR     GPIOB->BRR

#define LCDRS	           (1 << 13)
#define LCDEN	           (1 << 15)

#define LCD_D4 (1 << 7)
#define LCD_D5 (1 << 6)
#define LCD_D6 (1 << 5)
#define LCD_D7 (1 << 4)

#define NUM_OF_CHARS_PER_LINE 16
#define NUM_OF_LINES 2

#define LCD_DATA_MASK           (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)
#define BYTE_DELAY 5
#define DATA_POS 4

void lcd_command_write( unsigned char command )
{
  unsigned char temp=0;
  unsigned int temp1=0;

  temp=command;
  temp=(temp>>4)&0x0F;
  temp1=(temp<<DATA_POS)&LCD_DATA_MASK;

  LCD_CTRL_CLR = LCDRS; //clr
  LCD_CTRL_SET = LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK; //clr
  LCD_DATA_SET = temp1;
  delay_ms(BYTE_DELAY);
  LCD_CTRL_CLR = LCDEN; // clr

  temp=command;
  temp&=0x0F;
  temp1=(temp<<DATA_POS)&LCD_DATA_MASK;  

  LCD_CTRL_CLR = LCDRS;
  LCD_CTRL_SET = LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay_ms(1);
  LCD_CTRL_CLR = LCDEN;
  delay_ms(BYTE_DELAY);
}

void lcd_data_write( unsigned char data )
{
  unsigned char temp=0;
  unsigned int temp1=0;

  temp=data;
  temp=(temp>>4)&0x0F;
  temp1=(temp<<DATA_POS)&LCD_DATA_MASK;

  LCD_CTRL_SET = LCDEN|LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay_ms(BYTE_DELAY);
  LCD_CTRL_CLR = LCDEN;

  temp=data;
  temp&=0x0F;
  temp1=(temp<<DATA_POS)&LCD_DATA_MASK;

  LCD_CTRL_SET = LCDEN|LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay_ms(BYTE_DELAY);
  LCD_CTRL_CLR = LCDEN;
}

void lcd_clear( void)
{
  lcd_command_write( 0x01 );
}

int lcd_gotoxy( unsigned int x, unsigned int y)
{
  int retval = 0;

  if( (x > NUM_OF_LINES) && (y > NUM_OF_CHARS_PER_LINE) )
  {
    retval = -1;
  } 
  else 
  {
    if( x == 0 )
    {
      lcd_command_write( 0x80 + y );		/* command - position cursor at 0x00 (0x80 + 0x00 ) */
    } 
    else if( x == 1 )
    {      
      lcd_command_write( 0xC0 + y );		/* command - position cursor at 0x40 (0x80 + 0x00 ) */
    }
    else if( x == 2 )
    {      
      lcd_command_write( 0x94 + y );		/* command - position cursor at 0x40 (0x80 + 0x00 ) */
    }
    else if( x == 3 )
    {      
      lcd_command_write( 0xD4 + y );		/* command - position cursor at 0x40 (0x80 + 0x00 ) */
    }
  }
  return retval;
}

void lcd_putchar( int c )
{
  lcd_data_write( c );
}

void lcd_puts(char *string)
{
  unsigned char len = NUM_OF_CHARS_PER_LINE;  
  while(*string != '\0' && len--)
  {
    lcd_putchar( *string );		
    string++;
  }
}

void lcd_init( void )
{
  uint32_t cnt;
  GPIOB->MODER &= ~(((uint32_t)3 << (13 * 2)) | ((uint32_t)3 << (14 * 2)) | ((uint32_t)3 << (15 * 2)));
  GPIOB->MODER |= (1 << (13 * 2)) | (1 << (14 * 2)) | (1 << (15 * 2));
  // Enable pullup
  GPIOB->PUPDR |= (1 << (13 * 2)) | (1 << (14 * 2)) | (1 << (15 * 2));
  // Set port in high speed mode
  GPIOB->OSPEEDR |= (1 << (13 * 2)) | (1 << (14 * 2)) | (1 << (15 * 2));
	
	GPIOC->MODER &= ~((3 << (4 * 2)) | (3 << (5 * 2)) | (3 << (6 * 2)) | (3 << (7 * 2)));
	GPIOC->MODER |= (1 << (4 * 2)) | (1 << (5 * 2)) | (1 << (6 * 2)) | (1 << (7 * 2));
  // Enable pullup
  GPIOC->PUPDR |= (1 << (4 * 2)) | (1 << (5 * 2)) | (1 << (6 * 2)) | (1 << (7 * 2));
  // Set port in high speed mode
  GPIOC->OSPEEDR |= (1 << (4 * 2)) | (1 << (5 * 2)) | (1 << (6 * 2)) | (1 << (7 * 2));
  
  delay_ms(500);
  for(cnt = 0; cnt < 40; ++cnt)
  {
    lcd_command_write(0x22);     /*   4-bit interface, two line, 5X7 dots.        */
    delay_ms(10);
  }
	
	lcd_command_write(0x2C);
	delay_ms(50);
	lcd_command_write(0x08);
	delay_ms(50);
	lcd_command_write(0x01);
	delay_ms(50);
	lcd_command_write(0x06);
	delay_ms(50);
	lcd_command_write(0x14);
	delay_ms(50);
	lcd_command_write(0x0C);
	delay_ms(50);
	lcd_command_write(0x80);
	delay_ms(100);
}
