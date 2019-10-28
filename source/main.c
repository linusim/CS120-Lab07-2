#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#ifndef __MAIN_C__
#define __MAIN_C__

enum State{Start, Wait, Inc, Dec, Reset} state;
	
unsigned char button0; // declaration of button0 (A0)
unsigned char button1; // declaration of button1 (A1)
unsigned char tempC; // temp variable to store C
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

volatile unsigned char TimerFlag = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {TimerFlag = 1;}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void Tick() {
	switch(state) { // Transition actions
		case Start:
			state = Wait;
			break;
		case Wait:
			if(button0 && !button1) {
				state = Inc;
			} else if(!button0 && button1) {
				state = Dec;
			} else if(button0 && button1) {
				state = Reset;
			} else {
				state = Wait;
			}
			break;
		case Inc:
			if(button0 && !button1) {
				state = Inc;
			} else if(!button0 && button1) {
				state = Dec;
			} else if(button0 && button1) {
				state = Reset;
			} else {
				state = Wait;
			}
			break;
		case Dec:
			if(button0 && !button1) {
				state = Inc;
			} else if(!button0 && button1) {
				state = Dec;
			} else if(button0 && button1) {
				state = Reset;
			} else {
				state = Wait;
			}
			break;
		case Reset:
			if(button0 && button1) {
				state = Reset;
			} else {
				state = Wait;
			}
			break;
	}
	
	switch(state) { // State actions
		case Start:
			break;
		case Wait:
			break;
		case Inc:
			if(tempC < 9) {
				tempC = tempC + 1;
			}
			break;
		case Dec:
			if(tempC > 0) {
				tempC = tempC - 1;	
			}
			break;
		case Reset:
			tempC = 0;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0x00; // Configure A's 8 pins as inputs. Initialize to 0s.
	DDRC = 0xFF; PORTC = 0x00; // Configure C's 8 pins as outputs. Initialize to 0s. // LED data lines
	DDRD = 0xFF; PORTD = 0x00; // Configure D's 8 pins as outputs. Initialize to 0s. // LED control lines
		
	TimerSet(1000);
	TimerOn();
	
	LCD_init();
	LCD_ClearScreen();
	
	state = Start;
	tempC = 0x00;
	
    while (1) 
    {
		button0 = ~PINA & 0x01;
		button1 = ~PINA & 0x02;
		LCD_Cursor(1);
		LCD_WriteData(tempC + '0');
		Tick();
		while(!TimerFlag){}
		TimerFlag = 0;
    }
}


#endif
