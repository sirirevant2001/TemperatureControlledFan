#ifndef F_CPU
#define F_CPU 1600000UL //setting the clock speed of processor to 16Mhz, While the processor is running at 1MHz and you are defining 16MHz, it makes the program to run 16times slower. 
#endif
#include <avr/io.h>
#include <util/delay.h>
#include "LCD/lcd.h"

//initialising ADC_register
void adc_init()
{
	// AREF = AVcc// Our reference voltage will be Vcc(5v). So we set this.
	//this bit select the voltage reference for ADC, this is controlled by the REFS bit in the ADMUX register, here AVcc with external capacitor at AREF pin.
	ADMUX = (1<<REFS0);
	
	//all three ADPS bits must be set to get the 128 prescaler.
	//A prescaler of 128 will result in an ADC frequency of 125 khz, the prescaling is set by the ADPS bits in the ADCSRA register.
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); 
	
	//ADPS2-ADPS0 – These selects the Prescaler for ADC. As ADC frequency must be between 50KHz to 200KHz.
	//ADEN – Set this to 1 to enable ADC
}

// read adc value
uint16_t adc_read(uint8_t ch)
{
	//This line limits the channel number to a valid value.  
	//select the channel from 0~7, If you pass ch between 0-7 it remains unchanged but if you pass for example 11 it will be converted to 3
	ch &= 0b00000111; // can also be written as ch=ch&0b00000111
	
	//Sets the input channel to read from
	ADMUX = (ADMUX & 0xF8)|ch;
	
	
	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC); //We need to set this to one whenever we need ADC to do a conversion.
	
	// wait for conversion to complete
	// ADSC becomes '0' again
	while(ADCSRA & (1<<ADSC));//The loop does nothing while ADSC is set to 0, it exits as soon as ADSC is set to one, i.e. conversion is complete.
	return (ADC);
}

int main()
{
	DDRB=0xff;
	uint16_t adc_result0;
	int temp;
	char buffer[10];
	

	// initialize adc and lcd
	adc_init();
	lcd_init(LCD_DISP_ON_CURSOR); //CURSOR
	lcd_clrscr();
	lcd_gotoxy(0,0);
	_delay_ms(50);

	while(1)
	{
		adc_result0 = adc_read(0);      // read adc value at PA0
		temp=adc_result0/2.01;   // finding the temperature
		
		lcd_gotoxy(0,0);
		itoa(temp,buffer,10);
		lcd_puts("TEMP=");   //display temperature
		lcd_puts(buffer);
		lcd_gotoxy(7,0);
		lcd_puts("C");
		_delay_ms(1000);
		
		if(temp>=25)
		{
			lcd_clrscr();
			lcd_home();
			lcd_gotoxy(0,1);
			lcd_puts("FAN ON");
			
			PORTB=(1<<PINB0);
			
		}
		if (temp<25)
		{
			lcd_clrscr();
			lcd_home();
			lcd_gotoxy(7,1);
			lcd_puts("FAN OFF");
			
			PORTB=(0<<PINB0);
		}
	}
}