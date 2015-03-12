#define F_CPU 8000000UL
#define BAUD 1200
#define INTEGER 35
#define SPORT PORTB
#define SPIN PINB6

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include <string.h>

void play_sound(uint16_t freq, float duration);
void get_data(void);

//Sensor variables
volatile bool count = 0, flag = 0;
volatile uint16_t clockValue = 999;
volatile uint8_t i = 0;

void sendTrigger(void);

uint8_t data = 128;
char string[16] = {0};
char strdata[16] = {0};

int main()
{
    DDRB |= 1 << SPIN;
    
    data = 0;
    initalizeUART();

    TCCR1A |= 1 << COM1A1 | 1 << COM1B1 | 1 << WGM10;
    TIMSK1 |= 1 << TOIE1;
    TCCR1B |= 1 << CS11 | 1 << CS10;
    DDRB |= 1 << PINB1 | 1 << PINB2;
    
    //Initalize sensor
    DDRD |= 1 << PIND2;
    DDRD &= ~(1 << PIND3);

    EICRA |= 1 << ISC10;
    EIMSK |= 1 << INT1;


    TCCR0B |= 1 << CS02 | 1 << CS00;


    sei();
    play_sound(350,0.15);
    play_sound(400,0.15);
    play_sound(450,0.15);
    play_sound(500,0.15);
    play_sound(450,0.15);
    play_sound(500,0.30);

    while(1)
    {
        //_delay_ms(10);
        //transmit_sentence("je suis here");
        if(flag)
        {
          //  transmit_number(flag);
            OCR1A = 0;
            OCR1B = 0;

            strdata[0] = 'L';

            play_sound(450,0.25);
            play_sound(350,0.25);
            play_sound(450,0.25);
            play_sound(350,0.25);

            while(!((data < 10 || data > 245) && (strdata[0] == 'R')))
            {
                get_data();
            }
            if(data < 115)
            {
                OCR1A = 191*data/114;
                OCR1B = -64.0/115.0*data + 254;
            }
            else if(data > 141)
            {
                OCR1A = 64/114.0*data + 111;
                OCR1B = -1.67*data + 428;
            }
            play_sound(350,0.15);
            play_sound(400,0.15);
            play_sound(450,0.15);
            play_sound(500,0.15);
            play_sound(450,0.15);
            play_sound(500,0.30);
        }
        if(strdata[0] == 'R')
        {
            if(data < 115)
            {
                OCR1A = 191*data/114;
                OCR1B = -64.0/115.0*data + 254;
            }
            else if(data > 141)
            {
                OCR1A = 64/114.0*data + 111;
                OCR1B = -1.67*data + 428;
            }
            else
            {
                OCR1A = 191;
                OCR1B = 191;
            }
        }
        else
        {
           OCR1A = 0;
           OCR1B = 0;
        }
        get_data();
    }
}

ISR(USART_RX_vect)
{
    volatile char c = UDR0;
    _buffer[_k] = c;
    _k = (_k + 1)%4;
}

ISR(INT1_vect)
{   
    count ^= 1;
    if(!(bit_is_clear(PIND, 3))) TCNT0 = 0x00;
    else 
    {
        clockValue = TCNT0;
        if(clockValue*2.38 < 30) 
            flag = 1;
        else
            flag = 0;
    }
}

ISR(TIMER1_OVF_vect)
{
    i++;
    if(i == 100)
    {
        i = 0;
        sendTrigger();
    }
}


void sendTrigger(void)
{
    PORTD |= 1 << PIND2;
    _delay_us(10);
    PORTD &= ~(1 << PIND2);
}

void play_sound(uint16_t freq, float duration)
{
    volatile uint16_t i = duration*freq;
    for(int j = 0; j < i*2; j++)
    {
        SPORT ^= 1 << SPIN;
        for(uint16_t k = 0; k < 100000/freq/2; k++)
            _delay_us(10);
    }
}

void get_data(void)
{
    for(int j = 0; j < 4; j++)
    {
        if(_buffer[(3+j)%4] == (_buffer[(1+j)%4] + _buffer[(0+j)%4] + _buffer[(2+j)%4])%256 && (_buffer[(0+j)%4] == ADDRESS))
        {
            if(_buffer[(1+j)%4] == 35) data = (uint8_t)_buffer[(2+j)%4];
            else string[_buffer[(1+j)%4]] = _buffer[(2+j)%4];
            if(string[_buffer[(1+j)%4]] == '\0')
            {
                strcpy(strdata, string);
            }
            break;
        }
    }       
}
