/*
 * PI_controller_voltage_and_current_test3.c
 *
 * Created: 2021-10-13 오후 4:11:35
 * Author : kimhyeonsoo
 */
//////////////////// Proportional integral controller(load_voltage & inductor current) in Buck Converter with 16bit timer/counter  ////////////////////
//////////////////// Proportional integral controller(load_voltage & inductor current) in Buck Converter with 16bit timer/counter  ////////////////////
//////////////////// Proportional integral controller(load_voltage & inductor current) in Buck Converter with 16bit timer/counter  ////////////////////
//////////////////// Proportional integral controller(load_voltage & inductor current) in Buck Converter with 16bit timer/counter  ////////////////////
//==========================================================================================================================
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//==========================================================================================================================
float SETPOINT = 80.0;								//   0 ~ 100V control
float I_gain_voltage = 0.00010;						//   should be adjusted
float P_gain_voltage = 1.050000;					//   should be adjusted
float I_gain_current = 0.00030;						//   should be adjusted
float P_gain_current = 1.057800;					//   should be adjusted
float sampling_time_voltage = 0.00004;				//   should be adjusted
float sampling_time_current = 0.00002;				//   should be adjusted
//==========================================================================================================================
float cmd1 = 1.0;
float cmd2 = 0.0;
float cmd3 = 0.0;
float cmd4 = 0.0;
//==========================================================================================================================
float vv = 0.0;
float vv1 = 0.0;
float ii = 0.0;
float ii1 = 0.0;
float pp_v = 0.0;
float pp_i = 0.0;
//==========================================================================================================================
float load_voltage = 0.0;
float inductor_current = 0.0;
//==========================================================================================================================
unsigned int ADC2_RESULT = 0x0000;
unsigned int ADC3_RESULT = 0x0000;
//==========================================================================================================================
ISR(TIMER1_COMPA_vect){
	//* load voltage control system *//
	_delay_us(100);									//   be consumed 25 clocks to convert
	if((ADCSRA & 0x10) != 0x10) while(1);			//   ADIF = 0, if conversion is not completed
	ADC2_RESULT = 0x0000;
	ADC2_RESULT = ADCW;
	load_voltage = (float)ADC2_RESULT*5.0/1024.0;
	
	vv = vv+vv1*sampling_time_voltage;
	cmd1 = SETPOINT-load_voltage;
	vv1 = cmd1*I_gain_voltage;
	pp_v = cmd1*P_gain_voltage;
	cmd2 = vv+pp_v;
	
	//* inductor current control system *//
	ADCSRA = 0x00;									//   Initialization
	ADMUX = (0x00<<REFS0)|(0x03<<MUX0);		//   use ADC3
	ADCSRA = (0x01<<ADEN)|(0x01<<ADSC)|(0x00<<ADFR)|(0x04<<ADPS0);	//   AD Conversion start
	_delay_us(100);									//   be consumed 25 clocks to convert
	if((ADCSRA & 0x10) != 0x10) while(1);
	ADC3_RESULT = 0x0000;
	ADC3_RESULT = ADCW;
	inductor_current = (float)ADC3_RESULT*5.0/1024.0;
	
	ii = ii+ii1*sampling_time_current;
	cmd3 = cmd2-inductor_current;			//   cmd2 could be reference value of inductor_current
	ii1 = cmd3*I_gain_current;
	pp_i = cmd3*P_gain_current;
	cmd4 = ii+pp_i;			// cmd4 => pwminput
	
	OCR1A = 249*0.01*cmd4;
	//OCR1A = 100;
	ICR1 = 249;
}
//==========================================================================================================================
int main(void)
{	
	//   input/output setting
	DDRB = 0xff;					//   PWM output
	PORTB = 0x00;					//   Initialization
	DDRF = 0x00;					//   Analog input	
	DDRA = 0xff;					//   debug(led) output
	
	cli();
	ADMUX = (0x00 << REFS0) |(0x02 << MUX0);	//   ADC2 first						//   ADC2
	ADCSRA = (0x01 << ADEN) | (0x01 << ADSC) | (0x00 << ADFR) | (0x04 << ADPS0);	//   AD Conversion start
	TCCR1A = 0x82; TCCR1B = 0x1B;													//   ---> Fast PWM, 64 prescale ---> 250kHz --> 4us
	TIMSK = 0x10;					//   compare match interrupt enable															//   compare match interrupt enable	
	sei();							//   entire interrupt enable
	while(1)
	{
	}
}
//==========================================================================================================================

////if((OCR1A >= 220) & (OCR1A < 250)) PORTA = 0x01;
////if((OCR1A >= 200) & (OCR1A < 220)) PORTA = 0x02;
////if((OCR1A >= 180) & (OCR1A < 200)) PORTA = 0x04;
////if((OCR1A >= 160) & (OCR1A < 180)) PORTA = 0x08;
////if((OCR1A >= 130) & (OCR1A < 160)) PORTA = 0x10;
////if((OCR1A >= 100) & (OCR1A < 130)) PORTA = 0x20;
////if(OCR1A > 240)					   PORTA = 0x03;