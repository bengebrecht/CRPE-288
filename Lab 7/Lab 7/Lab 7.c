/*
 * Lab_7.c
 *
 * Created: 10/17/2014 12:17:53 PM
 *  Author: jsalinas
 */ 


#include <avr/io.h>
#include "util.h"
#include "lcd.h"
#include "open_interface.h"
#include <avr/interrupt.h>

volatile unsigned int redge;
volatile unsigned int fedge;
volatile int binary = 0;
volatile int mem = 0;
volatile char state;


void TC_init();
void send_pulse();
double time2dist(unsigned int time);

ISR (TIMER1_CAPT_vect){
	//State 0 = low
	if (state == 0){
			redge = ICR1;
			//State 1 = high
			state = 1;
			TCCR1B &= 0b10111111;
	}
	//State 1 = high
	else if(state == 1){
		//fedge = ICR1;
		fedge= 200;
		//State 2 = done
		state = 2;
		TCCR1B |= 0b01000000;
	}
	
}




int main(void)
{
	double distance = 0;
    lcd_init();
	TC_init();
	while(1){
		state = 0;
		cli();
		send_pulse();
		sei();
		while(state != 2);
		distance = time2dist((fedge-redge)/2);
		lprintf("Distance: %d", distance);
		wait_ms(200);
	}
	
}

void TC_init() {
	TCCR1A = 0b00000000;
	TCCR1B |= 0b01000100;
	//TCCR1C &= 0b00011111;
	TIMSK |= 0b00100100;
	TIFR |= 0b00111100;
	
	/*TCCR1A:	WGM	bits	=	0
	TCCR1B:	Enable	interrupt,	Choose	correct	Edge	Select,
	WGM	bits	=	0,	Choose	appropriate		Clock	Select
	TCCR1C:	Keep	all	bit	cleared
	TIMSK:	Enable	Timer/Counter	1	Input	Capture	Interrupt
	Port	D	pin	4	(PD4)	�	It	is	Timer1/Counter1�s	Input	Capture
	(IC)	pin,	and	connects	to	the	input/output	pin	of	the
	PING	sensor*/
}

void send_pulse()
{
	TIMSK &= 0b11011111;
	DDRD |=	0x10; //	set	PD4	as	output
	PORTD |= 0x10; //	set	PD4	to	high
	wait_ms(5); //	wait
	PORTD &= 0xEF; //	set	PD4	to	low
	DDRD &=	0xEF; //	set	PD4	as	input
	TIFR |= 0b00111100;
	TIMSK |= 0b00100000;

}

double time2dist(unsigned int time)
{
	double dist;
	dist = (double)time*((double)((256/16000000)*340000));
	return dist;
}