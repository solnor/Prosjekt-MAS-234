/*
 * BlinkingAvLED.cpp
 *
 * Created: 19/11/2020 12:45:35
 * Author : mathi
 */ 

#include <avr/io.h>
#include <util/delay.h>
//M� definere klokkefrekvensen til 1MHz:
#define F_CPU 1000000UL

void init()
{
	//Setter pin 5 som utgang (write)
	// 0b00000000 -> 0b00100000
	// 0b0001000
	DDRB |= 1 << PB1;
	
}

void loop()
{
	//Lager delay
	//Bruker busy-wait delay
	//Delay p� 100 ms
	for (int i = 0; i<10; ++i)
	{
		_delay_ms(100);
	}
	PORTB ^= 1 << PB1; //Toggler PB1 ved bruk 
	//av XOR operatoren ^=
}

int main(void)
{
	//Setter bare pin 5 til h�y
	init();
	PORTB |= 1 << PB1;
	
	
	while (1)
	{
		loop(); //Kj�rer denne s� LED'en blir togglet
	}
}
