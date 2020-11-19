/*
 * PWMdioder.cpp
 *
 * Created: 19/11/2020 12:10:03
 * Author : mathi
 */ 

#include <avr/io.h>
#define F_CPU 1000000UL //Definerer mikrokontrollerfrekvens,
//viktig for at delayfunksjonen skal fungere riktig
#include <util/delay.h> //Inkluderer delay



int main(void)
{
	/*
	Setter data direction for PB1 og PB2 til ut/HIGH
	COM1(A/B)1 setter pins PB1 og PB2 til 0 ved compare match av OCR1(A/B)
	WGM13-10 setter hvilken modus timeren er i, i denne
    modusen er timeren satt til 10bit-fastPWM med TOP på 1023
	CS10 setter prescaleren N til 1, 
	som gir den raskest mulige frekvensen i denne modusen.
	*/
    DDRB |= 1 << PB1 | 1 << PB2; 
	TCCR1A |= 1 << COM1A1 | 1 << COM1B1 | 1 << WGM11 | 1 << WGM10;
	TCCR1B |= 1 << WGM12 | 1 << CS10;
	OCR1A = 0;                        
	OCR1B = 1023;  //Starter OCR1A i 0 og OCR1B i 1023
	
	/*I while-loopen er det to for-looper som bytter på å 
	inkrementere/dekrementere OCR1(A/B), diodene 
	pulserer da 180 grader ute av fase
	*/
    while (1) 
    {
	for(int i = 0; i < 511; i++){
			OCR1A+=2;
			OCR1B-=2;
			_delay_ms(1);
		}
	for(int i = 0; i < 511; i++){
		OCR1A-=2;
		OCR1B+=2;
		_delay_ms(1);
	}
	}
}
