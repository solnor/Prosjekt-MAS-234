/*
 * StyringAvDioder.cpp
 *
 * Created: 19/11/2020 12:12:12
 * Author : mathi
 */ 

#include <avr/io.h>

int main(void)
{
	DDRB |= 1 << PB1 | 1 << PB2; //Setter data direction til output
	PORTB |= 0 << PB1 | 0 << PB2;//Setter pins PB1 og PB2 til LOW,
	//diodene er satt til sinking, de vil lyse.
	while (1)
	{
	}
}

