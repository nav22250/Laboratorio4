//*****************************************************************************
//Universidad del Valle de Guatemala
//IE2023 Programacionde Microcontroladores
//Author : Nicole Navas
//Proyecto: laboratorio 4: Programacion en C 
//IDescripcion: Codigo de contador de 8 bits y manejo de display con 
//				potenciometro
//Hardware: ATMega328P
//Created: 4/9/2024 10:08:47 PM
//Actualizado: 
//*****************************************************************************
// Encabezado
//*****************************************************************************

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t contador = 0; //iniciar el contador en 0
volatile uint8_t vADC = 0; //inicar contador del ADC
volatile uint8_t valor_disp1 = 0; //valor display1 
volatile uint8_t valor_disp2 = 0; //valor display2

const uint8_t milista[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78,  0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E};
	
void setup() {
	
	DDRC &= ~((1 << PC4) | (1 << PC5)| (1 << PC6)); // Configurar PC4 y PC5 como entradas

	PORTC |= (1 << PC4) | (1 << PC5); // Habilitar resistencias pull-up en PC4 y PC5

	PCICR |= (1 << PCIE1); 
	PCMSK1 |= (1 << PCINT12) | (1 << PCINT13);
	
	DDRD = 0xFF;
	PORTD = 0;
	UCSR0B = 0;
	
	DDRC = 0x0F;
	DDRB = 0xFF; //transistores como salidas

	sei(); // Habilitar interrupciones 
}

ISR(ADC_vect){
	
	vADC = ADCH;
	
	valor_disp1 = vADC & 0b00001111;
	valor_disp2 = vADC >> 4;
	
	ADCSRA |= (1<<ADIF);
	
	
}
ISR(PCINT1_vect) { //Interrupcion de botones
	
	_delay_ms(10);
	
	if (!(PINC & (1 << PC4))) { // comprobar si boton de incremento esta presionado
		contador++;
	}
	
	
	if (!(PINC & (1 << PC5))) { // comprobar si boton de decremento esta presionado
		contador--;
	}

}
 void initADC(void){
	 ADMUX = 0;
	 ADMUX |= (1 << REFS0); // referencia AVCC = 5V
	 
	 ADMUX |= (1 << ADLAR);
	 ADMUX |= (1 << MUX2) | (1 << MUX1)|(1 << MUX0); // Seleccionar canal ADC7 
	 
	 ADCSRA = 0;
	 ADCSRA |= (1<<ADIE); // habilitar interrupción del ADC
	 ADCSRA |= (1<<ADEN); // habilitar ADC
	 ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // habilitación de prescaler
	 
	 DIDR0 &= ~((1<<ADC5D)|(1<<ADC4D));
 }
 
int main() {
	cli();
	setup();
	initADC();
	sei();
	
	while (1) {
		ADCSRA |= (1<<ADSC);
		
		//actualizacion de LEDS
		PORTC = (PORTC & 0xF0) | (contador & 0x0F); // poner los primeros 4 bits en el PORTC
		PORTB = (PORTB & 0xF0) | ((contador & 0xF0) >> 4); // poner los ultimos leds en el PORTB
		
		PORTB |= (1<<PB4); 
		PORTB &= ~(1<<PB5);
		PORTD = (PORTD & 0b10000000)|(milista[valor_disp1]);
		_delay_ms(4);
		PORTB |= (1<<PB5);
		PORTB &= ~(1<<PB4);
		PORTD = (PORTD & 0b10000000)|(milista[valor_disp2]);
		_delay_ms(4);
		
		// Verificar si vADC es mayor que contador
		if (vADC > contador) {
			// Encender LED en PD7
			PORTD |= (1 << PD7);
			} else {
			// Apagar LED en PD7
			PORTD &= ~(1 << PD7);
		}
		
	}

	return 0; //retorno 
}
