#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define Target_Ticks 1000 // For 1 Second
#define Seg_Write(digit) ( PORTC = (PORTC & 0xF0) | (digit & 0x0F) )

/* Nums using in Display Numbers in 7 Segment From (1st ~ 6th) */
unsigned char num1 = 0 , num2 = 0 , num3 = 0 , num4 = 0 ;
unsigned char num5 = 0 , num6 = 0 ;

void INT0_Init(void)
{
	DDRD &= ~(1<<PD2); //Reset Button as Input
	PORTD |= (1<<PD2); // Enable Internal Pull Up

	MCUCR |= (1<<ISC01); // Falling Edge
	GICR |= (1<<INT0); // Enable Interrupt 0
}

void INT1_Init(void)
{
	DDRD &= ~(1<<PD3); //Paused Button as Input

	MCUCR = (1<<ISC10) | (1<<ISC11); // Raising Edge
	GICR |= (1<<INT1); // Enable Interrupt 1
}

void INT2_Init(void)
{
	DDRB &= ~(1<<PB2); //Resume Button as Input
	PORTB |= (1<<PB2); // Enable Internal Pull Up

	MCUCSR &= ~(1<<ISC2); // Falling Edge
	GICR |= (1<<INT2); // Enable Interrupt 2
}

void TIMER1C_Init(unsigned short Ticks)
{
	TCCR1A |= (1<<FOC1A); // Non PWM
	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12); //CTC - Prescaler = 1024
	TCNT1 = 0 ; //Initial Value
	OCR1A = Ticks ; // Value Which be compared
	TIMSK |= (1<<OCIE1A); // Enable Timer1A Compare
}

ISR(INT0_vect) // Reset Timer 1
{
	num1 = 0 ;
	num2 = 0 ;
	num3 = 0 ;
	num4 = 0 ;
	num5 = 0 ;
	num6 = 0 ;
	TCNT1 = 0 ; //Initial Value
	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12); //CTC - Prescaler = 1024
}

ISR(INT1_vect) // Paused Timer 1
{
	TCCR1B = 0 ; // No Clock Source For Timer 1
}

ISR(INT2_vect) // Resume Timer 1
{
	TCCR1B = (1<<CS10) | (1<<CS12) | (1<<WGM12); //CTC - Prescaler = 1024
}

ISR(TIMER1_COMPA_vect) // Interrupt occurs every 1 Second
{
	/* For Seconds Increasing */
	num1++;

	if(num1 == 10)
	{
		num1 = 0 ;
		num2++ ;
	}

	/* For Minutes increasing */
	if(num1 == 0 && num2 == 6)
	{
		num3++ ;
		num2 = 0 ;
	}

	if(num3 == 10)
	{
		num3 = 0 ;
		num4++ ;
	}

	/* For Hours Increasing */
	if(num3 == 0 && num4 == 6)
	{
		num5++;
		num4 = 0 ;
	}

	if(num5 == 10)
	{
		num5 = 0;
		num6++ ;
	}
}

void Segmnet_En(void)
{
	DDRA |= 0x3F ; //(PA0 ~ PA5) Output Pins
	PORTA &= 0xC0 ; // (PA0 ~ PA5) => Output = 0

	DDRC |= 0x0F; // (PC0 ~ PC3) Output Pins
}

int main(void)
{
	SREG |= (1<<7) ; //Enable General Interrupt
	INT0_Init(); // External Interrupt 0
	INT1_Init(); // External Interrupt 1
	INT2_Init(); // External Interrupt 2
	TIMER1C_Init(Target_Ticks); // Timer 1 CTC
	Segmnet_En(); // Enable 7 Segment

	while(1)
	{
		/* Write on 1st Segment */
		PORTA = (PORTA & 0xC0) | (1<<PA5) ;
		Seg_Write(num1);
		_delay_ms(2);

		/* Write on 2nd Segment */
		PORTA = (PORTA & 0xC0) | (1<<PA4) ;
		Seg_Write(num2);
		_delay_ms(2);

		/* Write on 3rd Segment */
		PORTA = (PORTA & 0xC0) | (1<<PA3) ;
		Seg_Write(num3);
		_delay_ms(2);

		/* Write on 4th Segment */
		PORTA = (PORTA & 0xC0) | (1<<PA2) ;
		Seg_Write(num4);
		_delay_ms(2);

		/* Write on 5th Segment */
		PORTA = (PORTA & 0xC0) | (1<<PA1) ;
		Seg_Write(num5);
		_delay_ms(2);

		/* Write on 6th Segment */
		PORTA = (PORTA & 0xC0) | (1<<PA0) ;
		Seg_Write(num6);
		_delay_ms(2);
	}
}
